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

#ifndef BACKENDS_BMV2_SIMPLE_SUME_SWITCH_SIMPLESUMESWITCH_H_
#define BACKENDS_BMV2_SIMPLE_SUME_SWITCH_SIMPLESUMESWITCH_H_

// #include <algorithm>
// #include <cstring>
// #include "frontends/common/constantFolding.h"
// #include "frontends/p4/evaluator/evaluator.h"
// #include "frontends/p4/simplify.h"
// #include "frontends/p4/unusedDeclarations.h"
// #include "midend/convertEnums.h"
#include "backends/bmv2/common/action.h"
#include "backends/bmv2/common/backend.h"
#include "backends/bmv2/common/control.h"
#include "backends/bmv2/common/deparser.h"
// #include "backends/bmv2/common/extern.h"
#include "backends/bmv2/common/globals.h"
#include "backends/bmv2/common/header.h"
#include "backends/bmv2/common/parser.h"
#include "backends/bmv2/common/programStructure.h"
// #include "backends/bmv2/common/sharedActionSelectorCheck.h"

namespace BMV2 {

class SumeProgramStructure : public ProgramStructure {
 public:
    std::set<cstring>                pipeline_controls;
    std::set<cstring>                non_pipeline_controls;

    const IR::P4Parser* parser;
    const IR::P4Control* pipeline;
    const IR::P4Control* deparser;

    // architecture related information
    ordered_map<const IR::Node*, block_t> block_type;

    SumeProgramStructure() { }
};

class SimpleSumeSwitchExpressionConverter : public ExpressionConverter {
    SumeProgramStructure* structure;

 public:
    SimpleSumeSwitchExpressionConverter(P4::ReferenceMap* refMap, P4::TypeMap* typeMap,
        SumeProgramStructure* structure, cstring scalarsName) :
        ExpressionConverter(refMap, typeMap, structure, scalarsName), structure(structure) { }

    void modelError(const char* format, const IR::Node* node) {
        ::error(format, node);
        ::error("Are you using an up-to-date sume_switch.p4?");
    }

    bool isSumeMetadataParameter(const IR::Parameter* param) {
        auto st = dynamic_cast<SumeProgramStructure*>(structure);
        auto params = st->parser->getApplyParameters();
        // param->dbprint(std::cout);
        if (params->size() != 5) {
            modelError("%1%: Expected 5 parameter for parser", st->parser);
            return false;
        }
        if (params->parameters.at(4) == param)
            return true;

        params = st->pipeline->getApplyParameters();
        if (params->size() != 4) {
            modelError("%1%: Expected 4 parameter for pipeline", st->pipeline);
            return false;
        }
        if (params->parameters.at(3) == param)
            return true;

        params = st->deparser->getApplyParameters();
        if (params->size() != 5) {
            modelError("%1%: Expected 5 parameter for deparser", st->deparser);
            return false;
        }
        if (params->parameters.at(4) == param)
            return true;

        return false;
    }

    Util::IJson* convertParam(const IR::Parameter* param, cstring fieldName) override {
        if (isSumeMetadataParameter(param)) {
            std::cout << "is sume metadata field: " << fieldName << std::endl;
            auto result = new Util::JsonObject();
            result->emplace("type", "field");
            auto e = BMV2::mkArrayField(result, "value");
            e->append("sume_metadata_t");
            e->append(fieldName);
            return result;
        }
        return nullptr;
    }
};

class ParseSumeArchitecture : public Inspector {
    SumeProgramStructure* structure;

 public:
    explicit ParseSumeArchitecture(SumeProgramStructure* structure) :
        structure(structure) { }
    void modelError(const char* format, const IR::Node* node);
    bool preorder(const IR::PackageBlock* block) override;
};

class DiscoverSumeStructure : public DiscoverStructure {
    SumeProgramStructure* structure;

 public:
    explicit DiscoverSumeStructure(SumeProgramStructure* structure)
        : DiscoverStructure(structure), structure(structure) {
        CHECK_NULL(structure);
        setName("InspectSumeProgram");
    }

    void postorder(const IR::P4Parser* p) override {
        if (structure->block_type.count(p)) {
            auto info = structure->block_type.at(p);
            if (info == PARSER) {
                structure->parser = p;
            }
        }
    }

    void postorder(const IR::P4Control* c) override {
        if (structure->block_type.count(c)) {
            auto info = structure->block_type.at(c);
            if (info == PIPELINE)
                structure->pipeline = c;
            else if (info == DEPARSER)
                structure->deparser = c;
        }
    }
};

class SimpleSumeSwitchBackend : public Backend {
    BMV2Options&        options;
    SumeProgramStructure* structure;

 public:
    void createActions(ConversionContext* ctxt, SumeProgramStructure* structure);

    void convert(const IR::ToplevelBlock* tlb) override;
    SimpleSumeSwitchBackend(BMV2Options& options, P4::ReferenceMap* refMap, P4::TypeMap* typeMap,
                        P4::ConvertEnums::EnumMapping* enumMap) :
        Backend(options, refMap, typeMap, enumMap), options(options) { }
};

}  // namespace BMV2

#endif /* BACKENDS_BMV2_SIMPLE_SUME_SWITCH_SIMPLESUMESWITCH_H_ */

