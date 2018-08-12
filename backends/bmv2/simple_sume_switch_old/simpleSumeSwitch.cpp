/*
Copyright 2013-present Barefoot Networks, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/**
 * This file implements the simple switch model
 */

#include <algorithm>
#include <cstring>
#include <set>
#include "simpleSumeSwitch.h"

using BMV2::mkArrayField;
using BMV2::mkParameters;
using BMV2::mkPrimitive;
using BMV2::nextId;
using BMV2::stringRepr;

namespace BMV2 {

// void ParseSumeArchitecture::modelError(const char* format, const IR::Node* node) {
//     ::error(format, node);
//     ::error("Are you using an up-to-date sume_switch.p4?");
// }

// bool ParseSumeArchitecture::preorder(const IR::PackageBlock* main) {
//     auto prsr = main->findParameterValue(v1model.sw.parser.name);
//     if (prsr == nullptr || !prsr->is<IR::ParserBlock>()) {
//         modelError("%1%: main package  match the expected model", main);
//         return false;
//     }
//     structure->block_type.emplace(prsr->to<IR::ParserBlock>()->container, V1_PARSER);

//     auto ingress = main->findParameterValue(v1model.sw.ingress.name);
//     if (ingress == nullptr || !ingress->is<IR::ControlBlock>()) {
//         modelError("%1%: main package does not match the expected model", main);
//         return false;
//     }
//     auto ingress_name = ingress->to<IR::ControlBlock>()->container->name;
//     structure->block_type.emplace(ingress->to<IR::ControlBlock>()->container, V1_INGRESS);
//     structure->pipeline_controls.emplace(ingress_name);

//     auto verify = main->findParameterValue(v1model.sw.verify.name);
//     if (verify == nullptr || !verify->is<IR::ControlBlock>()) {
//         modelError("%1%: main package does not match the expected model", main);
//         return false;
//     }
//     structure->block_type.emplace(verify->to<IR::ControlBlock>()->container, V1_VERIFY);
//     structure->non_pipeline_controls.emplace(verify->to<IR::ControlBlock>()->container->name);

//     auto egress = main->findParameterValue(v1model.sw.egress.name);
//     if (egress == nullptr || !egress->is<IR::ControlBlock>()) {
//         modelError("%1%: main package does not match the expected model", main);
//         return false;
//     }
//     auto egress_name = egress->to<IR::ControlBlock>()->container->name;
//     structure->block_type.emplace(egress->to<IR::ControlBlock>()->container, V1_EGRESS);
//     structure->pipeline_controls.emplace(egress_name);

//     auto compute = main->findParameterValue(v1model.sw.compute.name);
//     if (compute == nullptr || !compute->is<IR::ControlBlock>()) {
//         modelError("%1%: main package does not match the expected model", main);
//         return false;
//     }
//     structure->block_type.emplace(compute->to<IR::ControlBlock>()->container, V1_COMPUTE);
//     structure->non_pipeline_controls.emplace(compute->to<IR::ControlBlock>()->container->name);

//     auto deparser = main->findParameterValue(v1model.sw.deparser.name);
//     if (deparser == nullptr || !deparser->is<IR::ControlBlock>()) {
//         modelError("%1%: main package  match the expected model", main);
//         return false;
//     }
//     structure->block_type.emplace(deparser->to<IR::ControlBlock>()->container, V1_DEPARSER);
//     structure->non_pipeline_controls.emplace(deparser->to<IR::ControlBlock>()->container->name);

//     return false;
// }

// void
// SimpleSumeSwitchBackend::modelError(const char* format, const IR::Node* node) const {
//     ::error(format, node);
//     ::error("Are you using an up-to-date sume_switch.p4?");
// }

// cstring
// SimpleSumeSwitchBackend::createCalculation(cstring algo, const IR::Expression* fields,
//                                 Util::JsonArray* calculations, bool withPayload,
//                                 const IR::Node* sourcePositionNode = nullptr) {
//     cstring calcName = refMap->newName("calc_");
//     auto calc = new Util::JsonObject();
//     calc->emplace("name", calcName);
//     calc->emplace("id", nextId("calculations"));
//     if (sourcePositionNode != nullptr)
//         calc->emplace_non_null("source_info", sourcePositionNode->sourceInfoJsonObj());
//     calc->emplace("algo", algo);
//     if (!fields->is<IR::ListExpression>()) {
//         // expand it into a list
//         auto list = new IR::ListExpression({});
//         auto type = typeMap->getType(fields, true);
//         if (!type->is<IR::Type_StructLike>()) {
//             modelError("%1%: expected a struct", fields);
//             return calcName;
//         }
//         for (auto f : type->to<IR::Type_StructLike>()->fields) {
//             auto e = new IR::Member(fields, f->name);
//             auto ftype = typeMap->getType(f);
//             typeMap->setType(e, ftype);
//             list->push_back(e);
//         }
//         fields = list;
//         typeMap->setType(fields, type);
//     }
//     auto jright = conv->convertWithConstantWidths(fields);
//     if (withPayload) {
//         auto array = jright->to<Util::JsonArray>();
//         BUG_CHECK(array, "expected a JSON array");
//         auto payload = new Util::JsonObject();
//         payload->emplace("type", "payload");
//         payload->emplace("value", (Util::IJson*)nullptr);
//         array->append(payload);
//     }
//     calc->emplace("input", jright);
//     calculations->append(calc);
//     return calcName;
// }

// void
// SimpleSumeSwitchBackend::convertChecksum(const IR::BlockStatement *block, Util::JsonArray* checksums,
//                                      Util::JsonArray* calculations, bool verify) {
//     if (errorCount() > 0)
//         return;
//     for (auto stat : block->components) {
//         if (auto blk = stat->to<IR::BlockStatement>()) {
//             convertChecksum(blk, checksums, calculations, verify);
//             continue;
//         } else if (auto mc = stat->to<IR::MethodCallStatement>()) {
//             auto mi = P4::MethodInstance::resolve(mc, refMap, typeMap);
//             if (auto em = mi->to<P4::ExternFunction>()) {
//                 cstring functionName = em->method->name.name;
//                 if ((verify && (functionName == v1model.verify_checksum.name ||
//                                 functionName == v1model.verify_checksum_with_payload.name)) ||
//                     (!verify && (functionName == v1model.update_checksum.name ||
//                                  functionName == v1model.update_checksum_with_payload.name))) {
//                     bool usePayload = functionName.endsWith("_with_payload");
//                     if (mi->expr->arguments->size() != 4) {
//                         modelError("%1%: Expected 4 arguments", mc);
//                         return;
//                     }
//                     auto cksum = new Util::JsonObject();
//                     auto ei = P4::EnumInstance::resolve(
//                         mi->expr->arguments->at(3)->expression, typeMap);
//                     cstring algo = ExternConverter::convertHashAlgorithm(ei->name);
//                     cstring calcName = createCalculation(
//                         algo, mi->expr->arguments->at(1)->expression,
//                         calculations, usePayload, mc);
//                     cksum->emplace("name", refMap->newName("cksum_"));
//                     cksum->emplace("id", nextId("checksums"));
//                     // TODO(jafingerhut) - add line/col here?
//                     auto jleft = conv->convert(mi->expr->arguments->at(2)->expression);
//                     cksum->emplace("target", jleft->to<Util::JsonObject>()->get("value"));
//                     cksum->emplace("type", "generic");
//                     cksum->emplace("calculation", calcName);
//                     auto ifcond = conv->convert(
//                         mi->expr->arguments->at(0)->expression, true, false);
//                     cksum->emplace("if_cond", ifcond);
//                     checksums->append(cksum);
//                     continue;
//                 }
//             }
//         }
//         ::error("%1%: Only calls to %2% or %3% allowed", stat,
//                 verify ? v1model.verify_checksum.name : v1model.update_checksum.name,
//                 verify ? v1model.verify_checksum_with_payload.name :
//                 v1model.update_checksum_with_payload.name);
//     }
// }

// void SimpleSumeSwitchBackend::createActions(ConversionContext* ctxt, SumeProgramStructure* structure) {
//     auto cvt = new ActionConverter(ctxt, options.emitExterns);
//     for (auto it : structure->actions) {
//         auto action = it.first;
//         action->apply(*cvt);
//     }
// }

void
SimpleSumeSwitchBackend::convert(const IR::ToplevelBlock* tlb) {
    structure = new SumeProgramStructure();

    auto parseSumeArch = new ParseSumeArchitecture(structure);
    auto main = tlb->getMain();
    if (!main) return;  // no main
    main->apply(*parseSumeArch);
    if (::errorCount() > 0)
        return;

    /// Declaration which introduces the user metadata.
    /// We expect this to be a struct type.
    // TODO: replace with digest data
    // const IR::Type_Struct* userMetaType = nullptr;
    // cstring userMetaName = refMap->newName("userMetadata");

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

    // auto evaluator = new P4::EvaluatorPass(refMap, typeMap);
    // auto program = tlb->getProgram();
    // // These passes are logically bmv2-specific
    // PassManager simplify = {
    //     new RenameUserMetadata(refMap, userMetaType, userMetaName),
    //     new P4::ClearTypeMap(typeMap),  // because the user metadata type has changed
    //     new P4::SynthesizeActions(refMap, typeMap,
    //                               new SkipControls(&structure->non_pipeline_controls)),
    //     new P4::MoveActionsToTables(refMap, typeMap),
    //     new P4::TypeChecking(refMap, typeMap),
    //     new P4::SimplifyControlFlow(refMap, typeMap),
    //     new LowerExpressions(typeMap),
    //     new P4::ConstantFolding(refMap, typeMap, false),
    //     new P4::TypeChecking(refMap, typeMap),
    //     new RemoveComplexExpressions(refMap, typeMap,
    //                                  new ProcessControls(&structure->pipeline_controls)),
    //     new P4::SimplifyControlFlow(refMap, typeMap),
    //     new P4::RemoveAllUnusedDeclarations(refMap),
    //     evaluator,
    //     new VisitFunctor([this, evaluator]() { toplevel = evaluator->getToplevelBlock(); }),
    // };

    // auto hook = options.getDebugHook();
    // simplify.addDebugHook(hook);

    // program->apply(simplify);

    // // map IR node to compile-time allocated resource blocks.
    // toplevel->apply(*new BMV2::BuildResourceMap(&structure->resourceMap));

    // // field list and learn list ids in bmv2 are not consistent with ids for
    // // other objects: they need to start at 1 (not 0) since the id is also used
    // // as a "flag" to indicate that a certain simple_switch primitive has been
    // // called (e.g. resubmit or generate_digest)
    // BMV2::nextId("field_lists");
    // BMV2::nextId("learn_lists");
    // json->add_program_info(options.file);
    // json->add_meta_info();

    // // convert all enums to json
    // for (const auto &pEnum : *enumMap) {
    //     auto name = pEnum.first->getName();
    //     for (const auto &pEntry : *pEnum.second) {
    //         json->add_enum(name, pEntry.first, pEntry.second);
    //     }
    // }
    // if (::errorCount() > 0)
    //     return;

    // /// generate error types
    // for (const auto &p : structure->errorCodesMap) {
    //     auto name = p.first->toString();
    //     auto type = p.second;
    //     json->add_error(name, type);
    // }

    main = toplevel->getMain();
    if (!main) return;  // no main
    main->apply(*parseV1Arch);
    PassManager updateStructure {
        new DiscoverV1Structure(structure),
    };
    program = toplevel->getProgram();
    program->apply(updateStructure);

    cstring scalarsName = refMap->newName("scalars");
    // This visitor is used in multiple passes to convert expression to json
    conv = new SimpleSwitchExpressionConverter(refMap, typeMap, structure, scalarsName);

    auto ctxt = new ConversionContext(refMap, typeMap, toplevel, structure, conv, json);

    auto hconv = new HeaderConverter(ctxt, scalarsName);
    program->apply(*hconv);

    auto pconv = new ParserConverter(ctxt);
    structure->parser->apply(*pconv);

    createActions(ctxt, structure);

    auto cconv = new ControlConverter(ctxt, "pipeline", options.emitExterns);
    structure->pipeline->apply(*cconv);

    auto dconv = new DeparserConverter(ctxt);
    structure->deparser->apply(*dconv);

    // convertChecksum(structure->compute_checksum->body, json->checksums,
    //                 json->calculations, false);

    // convertChecksum(structure->verify_checksum->body, json->checksums,
    //                 json->calculations, true);

    (void)toplevel->apply(ConvertGlobals(ctxt, options.emitExterns));
}

}  // namespace BMV2
