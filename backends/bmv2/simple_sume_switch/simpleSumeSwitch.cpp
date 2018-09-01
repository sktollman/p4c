//
// Copyright (c) 2018 Sarah Tollman
// All rights reserved.
//
// This software was developed by Stanford University and the University of Cambridge Computer Laboratory
// under National Science Foundation under Grant No. CNS-0855268,
// the University of Cambridge Computer Laboratory under EPSRC INTERNET Project EP/H040536/1 and
// by the University of Cambridge Computer Laboratory under DARPA/AFRL contract FA8750-11-C-0249 ("MRC2"),
// as part of the DARPA MRC research programme.
//
// @NETFPGA_LICENSE_HEADER_START@
//
// Licensed to NetFPGA C.I.C. (NetFPGA) under one or more contributor
// license agreements.  See the NOTICE file distributed with this work for
// additional information regarding copyright ownership.  NetFPGA licenses this
// file to you under the NetFPGA Hardware-Software License, Version 1.0 (the
// "License"); you may not use this file except in compliance with the
// License.  You may obtain a copy of the License at:
//
//   http://www.netfpga-cic.org
//
// Unless required by applicable law or agreed to in writing, Work distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations under the License.
//
// @NETFPGA_LICENSE_HEADER_END@
//

/**
 * This file implements the simple sume switch model
 */

#include "simpleSumeSwitch.h"

namespace BMV2 {

bool ParseSumeArchitecture::preorder(const IR::PackageBlock* main) {
    auto prsr = main->findParameterValue("p");
    if (prsr == nullptr || !prsr->is<IR::ParserBlock>()) {
        return false;
    }
    structure->block_type.emplace(prsr->to<IR::ParserBlock>()->container, PARSER);

    auto pipeline = main->findParameterValue("map");
    if (pipeline == nullptr || !pipeline->is<IR::ControlBlock>()) {
        return false;
    }
    auto pipeline_name = pipeline->to<IR::ControlBlock>()->container->name;
    structure->block_type.emplace(pipeline->to<IR::ControlBlock>()->container, PIPELINE);
    structure->pipeline_controls.emplace(pipeline_name);

    auto deparser = main->findParameterValue("d");
    if (deparser == nullptr || !deparser->is<IR::ControlBlock>()) {
        return false;
    }
    structure->block_type.emplace(deparser->to<IR::ControlBlock>()->container, DEPARSER);
    structure->non_pipeline_controls.emplace(deparser->to<IR::ControlBlock>()->container->name);

    return false;
}

void SimpleSumeSwitchBackend::createActions(ConversionContext* ctxt, SumeProgramStructure* structure) {
    auto cvt = new ActionConverter(ctxt, options.emitExterns);
    for (auto it : structure->actions) {
        auto action = it.first;
        action->apply(*cvt);
    }
}

void
SimpleSumeSwitchBackend::convert(const IR::ToplevelBlock* tlb) {
    structure = new SumeProgramStructure();
    auto parseSumeArch = new ParseSumeArchitecture(structure);
    auto main = tlb->getMain();
    if (!main)
        return;  // no main
    main->apply(*parseSumeArch);
    if (::errorCount() > 0)
        return;

    // /// Declaration which introduces the user metadata.
    // /// We expect this to be a struct type.
    // const IR::Type_Struct* userMetaType = nullptr;
    // cstring userMetaName = refMap->newName("user_metadata_t");

    // // Find the user metadata declaration
    // auto parser = main->findParameterValue(v1model.sw.parser.name);
    // if (parser == nullptr) return;
    // if (!parser->is<IR::ParserBlock>()) {
    //     modelError("%1%: main package  match the expected model", main);
    //     return;
    // }
    // auto params = parser->to<IR::ParserBlock>()->container->getApplyParameters();
    // BUG_CHECK(params->size() == 4, "%1%: expected 4 parameters", parser);
    // auto metaParam = params->parameters.at(2);
    // auto paramType = metaParam->type;
    // if (!paramType->is<IR::Type_Name>()) {
    //     ::error("%1%: expected the user metadata type to be a struct", paramType);
    //     return;
    // }
    // auto decl = refMap->getDeclaration(paramType->to<IR::Type_Name>()->path);
    // if (!decl->is<IR::Type_Struct>()) {
    //     ::error("%1%: expected the user metadata type to be a struct", paramType);
    //     return;
    // }
    // userMetaType = decl->to<IR::Type_Struct>();
    // LOG2("User metadata type is " << userMetaType);

    auto evaluator = new P4::EvaluatorPass(refMap, typeMap);
    auto program = tlb->getProgram();
    // These passes are logically bmv2-specific
    PassManager simplify = {
        // new RenameUserMetadata(refMap, userMetaType, userMetaName),
        // new P4::ClearTypeMap(typeMap),  // because the user metadata type has changed
        new P4::SynthesizeActions(refMap, typeMap,
            new SkipControls(&structure->non_pipeline_controls)),
        new P4::MoveActionsToTables(refMap, typeMap),
        new P4::TypeChecking(refMap, typeMap),
        new P4::SimplifyControlFlow(refMap, typeMap),
        new LowerExpressions(typeMap),
        new P4::ConstantFolding(refMap, typeMap, false),
        new P4::TypeChecking(refMap, typeMap),
        new RemoveComplexExpressions(refMap, typeMap,
            new ProcessControls(&structure->pipeline_controls)),
        new P4::SimplifyControlFlow(refMap, typeMap),
        new P4::RemoveAllUnusedDeclarations(refMap),
        evaluator,
        new VisitFunctor([this, evaluator]() { toplevel = evaluator->getToplevelBlock(); }),
    };
    auto hook = options.getDebugHook();
    simplify.addDebugHook(hook);
    program->apply(simplify);

    // map IR node to compile-time allocated resource blocks.
    toplevel->apply(*new BMV2::BuildResourceMap(&structure->resourceMap));

    // field list and learn list ids in bmv2 are not consistent with ids for
    // other objects: they need to start at 1 (not 0) since the id is also used
    // as a "flag" to indicate that a certain simple_switch primitive has been
    // called (e.g. resubmit or generate_digest)
    BMV2::nextId("field_lists");
    BMV2::nextId("learn_lists");
    json->add_program_info(options.file);
    json->add_meta_info();

    // convert all enums to json
    for (const auto &pEnum : *enumMap) {
        auto name = pEnum.first->getName();
        for (const auto &pEntry : *pEnum.second) {
            json->add_enum(name, pEntry.first, pEntry.second);
        }
    }
    if (::errorCount() > 0)
        return;

    /// generate error types
    for (const auto &p : structure->errorCodesMap) {
        auto name = p.first->toString();
        auto type = p.second;
        json->add_error(name, type);
    }

    main = toplevel->getMain();
    if (!main)
        return;  // no main

    main->apply(*parseSumeArch);
    PassManager updateStructure {
        new DiscoverSumeStructure(structure),
    };
    program = toplevel->getProgram();
    program->apply(updateStructure);

    cstring scalarsName = refMap->newName("scalars");
    // This visitor is used in multiple passes to convert expression to json
    conv = new SimpleSumeSwitchExpressionConverter(refMap, typeMap, structure, scalarsName);

    auto ctxt = new ConversionContext(refMap, typeMap, toplevel, structure, conv, json);

    auto hconv = new HeaderConverter(ctxt, scalarsName, "sume_metadata_t");
    program->apply(*hconv);

    auto pconv = new ParserConverter(ctxt);
    structure->parser->apply(*pconv);

    createActions(ctxt, structure);

    auto cconv = new ControlConverter(ctxt, "TopPipe", options.emitExterns);
    structure->pipeline->apply(*cconv);

    auto dconv = new DeparserConverter(ctxt);
    structure->deparser->apply(*dconv);

    (void)toplevel->apply(ConvertGlobals(ctxt, options.emitExterns));
}

}  // namespace BMV2
