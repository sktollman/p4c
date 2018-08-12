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

#ifndef BACKENDS_BMV2_SIMPLE_SUME_SWITCH_SIMPLESUMESWITCH_H_
#define BACKENDS_BMV2_SIMPLE_SUME_SWITCH_SIMPLESUMESWITCH_H_

#include <algorithm>
#include <cstring>
#include "frontends/common/constantFolding.h"
#include "frontends/p4/evaluator/evaluator.h"
#include "frontends/p4/fromv1.0/v1model.h"
#include "frontends/p4/simplify.h"
#include "frontends/p4/unusedDeclarations.h"
#include "midend/convertEnums.h"
#include "backends/bmv2/common/action.h"
#include "backends/bmv2/common/backend.h"
#include "backends/bmv2/common/control.h"
#include "backends/bmv2/common/deparser.h"
#include "backends/bmv2/common/extern.h"
#include "backends/bmv2/common/globals.h"
#include "backends/bmv2/common/header.h"
#include "backends/bmv2/common/parser.h"
#include "backends/bmv2/common/programStructure.h"
#include "backends/bmv2/common/sharedActionSelectorCheck.h"

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

    bool isStandardMetadataParameter(const IR::Parameter* param) {
        auto st = dynamic_cast<V1ProgramStructure*>(structure);
        auto params = st->parser->getApplyParameters();
        if (params->size() != 4) {
            modelError("%1%: Expected 4 parameter for parser", st->parser);
            return false;
        }
        if (params->parameters.at(3) == param)
            return true;

        params = st->ingress->getApplyParameters();
        if (params->size() != 3) {
            modelError("%1%: Expected 3 parameter for ingress", st->ingress);
            return false;
        }
        if (params->parameters.at(2) == param)
            return true;

        params = st->egress->getApplyParameters();
        if (params->size() != 3) {
            modelError("%1%: Expected 3 parameter for egress", st->egress);
            return false;
        }
        if (params->parameters.at(2) == param)
            return true;

        return false;
    }

    Util::IJson* convertParam(const IR::Parameter* param, cstring fieldName) override {
        if (isStandardMetadataParameter(param)) {
            auto result = new Util::JsonObject();
            result->emplace("type", "field");
            auto e = BMV2::mkArrayField(result, "value");
            e->append("standard_metadata");
            e->append(fieldName);
            return result;
        }
        return nullptr;
    }
};

class ParseSumeArchitecture : public Inspector {
    SumeProgramStructure* structure;
    P4V1::V1Model&      v1model; // TODO: what replaces this?

 public:
    explicit ParseSumeArchitecture(SumeProgramStructure* structure) :
        structure(structure), v1model(P4V1::V1Model::instance) { }
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
            if (info == V1_PARSER) {
                structure->parser = p;
            }
        }
    }

    void postorder(const IR::P4Control* c) override {
        if (structure->block_type.count(c)) {
            auto info = structure->block_type.at(c);
            if (info == V1_INGRESS)
                structure->ingress = c;
            else if (info == V1_EGRESS)
                structure->egress = c;
            else if (info == V1_COMPUTE)
                structure->compute_checksum = c;
            else if (info == V1_VERIFY)
                structure->verify_checksum = c;
            else if (info == V1_DEPARSER)
                structure->deparser = c;
        }
    }
};

class SimpleSumeSwitchBackend : public Backend {
    BMV2Options&        options;
    P4V1::V1Model&      v1model;
    V1ProgramStructure* structure;

 protected:
    cstring createCalculation(cstring algo, const IR::Expression* fields,
                              Util::JsonArray* calculations, bool usePayload, const IR::Node* node);

 public:
    void modelError(const char* format, const IR::Node* place) const;
    void convertChecksum(const IR::BlockStatement* body, Util::JsonArray* checksums,
                         Util::JsonArray* calculations, bool verify);
    void createActions(ConversionContext* ctxt, V1ProgramStructure* structure);

    void convert(const IR::ToplevelBlock* tlb) override;
    SimpleSwitchBackend(BMV2Options& options, P4::ReferenceMap* refMap, P4::TypeMap* typeMap,
                        P4::ConvertEnums::EnumMapping* enumMap) :
        Backend(options, refMap, typeMap, enumMap), options(options),
        v1model(P4V1::V1Model::instance) { }
};

}  // namespace BMV2

#endif /* BACKENDS_BMV2_SIMPLE_SWITCH_SIMPLESWITCH_H_ */
