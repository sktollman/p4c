// /*
// Copyright 2013-present Barefoot Networks, Inc.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// */

// #ifndef BACKENDS_BMV2_SIMPLE_SUME_SWITCH_SIMPLESUMESWITCH_H_
// #define BACKENDS_BMV2_SIMPLE_SUME_SWITCH_SIMPLESUMESWITCH_H_

// #include "ir/ir.h"
// #include "lib/gmputil.h"
// #include "lib/json.h"
// #include "frontends/common/resolveReferences/referenceMap.h"
// #include "frontends/common/constantFolding.h"
// #include "frontends/p4/evaluator/evaluator.h"
// #include "frontends/p4/coreLibrary.h"
// #include "frontends/p4/enumInstance.h"
// #include "frontends/p4/methodInstance.h"
// #include "frontends/p4/typeMap.h"
// #include "frontends/p4/simplify.h"
// #include "frontends/p4/unusedDeclarations.h"
// #include "backends/bmv2/common/action.h"
// #include "backends/bmv2/common/control.h"
// #include "backends/bmv2/common/deparser.h"
// #include "backends/bmv2/common/extern.h"
// #include "backends/bmv2/common/header.h"
// #include "backends/bmv2/common/helpers.h"
// #include "backends/bmv2/common/lower.h"
// #include "backends/bmv2/common/parser.h"
// #include "backends/bmv2/common/programStructure.h"

// namespace BMV2 {

// class SimpleSumeSwitchExpressionConverter : public ExpressionConverter {
//  public:
//     SimpleSumeSwitchExpressionConverter(P4::ReferenceMap* refMap, P4::TypeMap* typeMap,
//                                       ProgramStructure* structure, cstring scalarsName) :
//     BMV2::ExpressionConverter(refMap, typeMap, structure, scalarsName) { }

//     Util::IJson* convertParam(UNUSED const IR::Parameter* param, cstring fieldName) override {
//         LOG3("convert " << fieldName);
//         return nullptr;
//     }
// };

// class SumeProgramStructure : public ProgramStructure {
//     P4::ReferenceMap*    refMap;
//     P4::TypeMap*         typeMap;

//  public:
//     // We place scalar user metadata fields (i.e., bit<>, bool)
//     // in the scalarsName metadata object, so we may need to rename
//     // these fields.  This map holds the new names.
//     std::vector<const IR::StructField*> scalars;
//     unsigned                            scalars_width = 0;
//     unsigned                            error_width = 32;
//     unsigned                            bool_width = 1;

//     // architecture related information
//     ordered_map<const IR::Node*, std::pair<gress_t, block_t>> block_type;

//     ordered_map<cstring, const IR::Type_Header*> header_types;
//     ordered_map<cstring, const IR::Type_Struct*> metadata_types;
//     ordered_map<cstring, const IR::Type_HeaderUnion*> header_union_types;
//     ordered_map<cstring, const IR::Declaration_Variable*> headers;
//     ordered_map<cstring, const IR::Declaration_Variable*> metadata;
//     ordered_map<cstring, const IR::Declaration_Variable*> header_stacks;
//     ordered_map<cstring, const IR::Declaration_Variable*> header_unions;
//     ordered_map<cstring, const IR::Type_Error*> errors;
//     ordered_map<cstring, const IR::Type_Enum*> enums;
//     ordered_map<cstring, const IR::P4Parser*> parsers;
//     ordered_map<cstring, const IR::P4ValueSet*> parse_vsets;
//     ordered_map<cstring, const IR::P4Control*> deparsers;
//     ordered_map<cstring, const IR::P4Control*> pipelines;
//     ordered_map<cstring, const IR::Declaration_Instance*> extern_instances;
//     ordered_map<cstring, cstring> field_aliases;

//     std::vector<const IR::ExternBlock*> globals;

//  public:
//     SumeProgramStructure(P4::ReferenceMap* refMap, P4::TypeMap* typeMap)
//         : refMap(refMap), typeMap(typeMap) {
//         CHECK_NULL(refMap);
//         CHECK_NULL(typeMap);
//     }

//     void create(ConversionContext* ctxt);
//     void createStructLike(ConversionContext* ctxt, const IR::Type_StructLike* st);
//     void createTypes(ConversionContext* ctxt);
//     void createHeaders(ConversionContext* ctxt);
//     void createParsers(ConversionContext* ctxt);
//     void createExterns();
//     void createActions(ConversionContext* ctxt);
//     void createControls(ConversionContext* ctxt);
//     void createDeparsers(ConversionContext* ctxt);
//     void createGlobals();

//     bool hasVisited(const IR::Type_StructLike* st) {
//         if (auto h = st->to<IR::Type_Header>())
//             return header_types.count(h->getName());
//         else if (auto s = st->to<IR::Type_Struct>())
//             return metadata_types.count(s->getName());
//         else if (auto u = st->to<IR::Type_HeaderUnion>())
//             return header_union_types.count(u->getName());
//         return false;
//     }
// };

// class ParseSumeArchitecture : public Inspector {
//     SumeProgramStructure* structure;
//  public:
//     explicit ParseSumeArchitecture(SumeProgramStructure* structure) :
//         structure(structure) { CHECK_NULL(structure); }

//     bool preorder(const IR::ToplevelBlock* block) override;
//     bool preorder(const IR::PackageBlock* block) override;
//     bool preorder(const IR::ExternBlock* block) override;

//     profile_t init_apply(const IR::Node *root) override {
//         structure->block_type.clear();
//         structure->globals.clear();
//         return Inspector::init_apply(root);
//     }
// };

// class InspectSumeProgram : public Inspector {
//     P4::ReferenceMap* refMap;
//     P4::TypeMap* typeMap;
//     SumeProgramStructure *pinfo;

//  public:
//     InspectSumeProgram(P4::ReferenceMap* refMap, P4::TypeMap* typeMap, SumeProgramStructure *pinfo)
//         : refMap(refMap), typeMap(typeMap), pinfo(pinfo) {
//         CHECK_NULL(refMap);
//         CHECK_NULL(typeMap);
//         CHECK_NULL(pinfo);
//         setName("InspectSumeProgram");
//     }

//     void postorder(const IR::P4Parser *p) override;
//     void postorder(const IR::P4Control* c) override;
//     void postorder(const IR::Declaration_Instance* di) override;

//     bool isHeaders(const IR::Type_StructLike* st);
//     void addTypesAndInstances(const IR::Type_StructLike* type, bool meta);
//     void addHeaderType(const IR::Type_StructLike *st);
//     void addHeaderInstance(const IR::Type_StructLike *st, cstring name);
//     bool preorder(const IR::Parameter* parameter) override;
// };

// class ConvertSumeToJson : public Inspector {
//  public:
//     P4::ReferenceMap *refMap;
//     P4::TypeMap *typeMap;
//     const IR::ToplevelBlock *toplevel;
//     JsonObjects *json;
//     SumeProgramStructure *structure;

//     ConvertSumeToJson(P4::ReferenceMap *refMap, P4::TypeMap *typeMap,
//                      const IR::ToplevelBlock *toplevel,
//                      JsonObjects *json, SumeProgramStructure *structure)
//         : refMap(refMap), typeMap(typeMap), toplevel(toplevel), json(json),
//           structure(structure) {
//         CHECK_NULL(refMap);
//         CHECK_NULL(typeMap);
//         CHECK_NULL(toplevel);
//         CHECK_NULL(json);
//         CHECK_NULL(structure); }

//     void postorder(UNUSED const IR::P4Program* program) override {
//         cstring scalarsName = refMap->newName("scalars");
//         // This visitor is used in multiple passes to convert expression to json
//         auto conv = new SimpleSumeSwitchExpressionConverter(refMap, typeMap, structure, scalarsName);
//         auto ctxt = new ConversionContext(refMap, typeMap, toplevel, structure, conv, json);
//         structure->create(ctxt);
//     }
// };

// class SimpleSumeSwitchBackend : public Backend {
//     BMV2Options &options;

//  public:
//     void convert(const IR::ToplevelBlock* tlb) override;
//     SimpleSumeSwitchBackend(BMV2Options& options, P4::ReferenceMap* refMap, P4::TypeMap* typeMap,
//                           P4::ConvertEnums::EnumMapping* enumMap) :
//         Backend(options, refMap, typeMap, enumMap), options(options) { }
// };

// }  // namespace BMV2

// #endif  /* BACKENDS_BMV2_SIMPLE_SUME_SWITCH_SUMESWITCH_H_ */

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
// #include "frontends/p4/fromv1.0/v1model.h"
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
    const IR::P4Control* ingress;
    const IR::P4Control* egress;
    const IR::P4Control* compute_checksum;
    const IR::P4Control* verify_checksum;
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

    // void modelError(const char* format, const IR::Node* node) {
    //     ::error(format, node);
    //     ::error("Are you using an up-to-date v1model.p4?");
    // }

    // bool isStandardMetadataParameter(const IR::Parameter* param) {
    //     auto st = dynamic_cast<SumeProgramStructure*>(structure);
    //     auto params = st->parser->getApplyParameters();
    //     if (params->size() != 4) {
    //         modelError("%1%: Expected 4 parameter for parser", st->parser);
    //         return false;
    //     }
    //     if (params->parameters.at(3) == param)
    //         return true;

    //     params = st->ingress->getApplyParameters();
    //     if (params->size() != 3) {
    //         modelError("%1%: Expected 3 parameter for ingress", st->ingress);
    //         return false;
    //     }
    //     if (params->parameters.at(2) == param)
    //         return true;

    //     params = st->egress->getApplyParameters();
    //     if (params->size() != 3) {
    //         modelError("%1%: Expected 3 parameter for egress", st->egress);
    //         return false;
    //     }
    //     if (params->parameters.at(2) == param)
    //         return true;

    //     return false;
    // }

    Util::IJson* convertParam(const IR::Parameter* param, cstring fieldName) override {
        // if (isStandardMetadataParameter(param)) {
        //     auto result = new Util::JsonObject();
        //     result->emplace("type", "field");
        //     auto e = BMV2::mkArrayField(result, "value");
        //     e->append("standard_metadata");
        //     e->append(fieldName);
        //     return result;
        // }
        return nullptr;
    }
};

class ParseSumeArchitecture : public Inspector {
    SumeProgramStructure* structure;
    // P4V1::V1Model&      v1model;

 public:
    explicit ParseSumeArchitecture(SumeProgramStructure* structure) :
        structure(structure)/*, v1model(P4V1::V1Model::instance)*/ { }
    void modelError(const char* format, const IR::Node* node);
    bool preorder(const IR::PackageBlock* block) override;
};

class DiscoverSumeStructure : public DiscoverStructure {
    SumeProgramStructure* structure;

 public:
    explicit DiscoverSumeStructure(SumeProgramStructure* structure)
        : DiscoverStructure(structure), structure(structure) {
        CHECK_NULL(structure);
        // std::cout << "initializing discover sume structure" << std::endl;
        setName("InspectSumeProgram");
    }

    void postorder(const IR::P4Parser* p) override {
        // std::cout << "in post order 1" << std::endl;
        // for(auto elem : structure->block_type)
        // {
        //    std::cout << elem.first << " " << elem.second << " " << elem.second << "\n";
        // }
        // std::cout << "block size: " << structure->block_type.size() << std::endl;
        // std::cout << "count: " << structure->block_type.count(p) << std::endl;
        // p->dbprint(std::cout);
        if (structure->block_type.count(p)) {
            // std::cout << "yes count" << std::endl;
            auto info = structure->block_type.at(p);
            // std::cout << "info: " << info << std::endl;
            if (info == PARSER) {
                // std::cout << "setting parser" << std::endl;
                structure->parser = p;
            }
        }
    }

    void postorder(const IR::P4Control* c) override {
        // std::cout << "in post order 2" << std::endl;
        // std::cout << "count: " << structure->block_type.count(c) << std::endl;
        // c->dbprint(std::cout);
        if (structure->block_type.count(c)) {
            // std::cout << "yes count" << std::endl;
            auto info = structure->block_type.at(c);
            // std::cout << "info: " << info << std::endl;
            if (info == PIPELINE) {
                // std::cout << "setting ingress" << std::endl;
                structure->ingress = c;
            }
            // else if (info == V1_EGRESS)
            //     structure->egress = c;
            // else if (info == V1_COMPUTE)
            //     structure->compute_checksum = c;
            // else if (info == V1_VERIFY)
            //     structure->verify_checksum = c;
            else if (info == DEPARSER) {
                // std::cout << "setting deparser" << std::endl;
                structure->deparser = c;
            }
        }
    }
};

class SimpleSumeSwitchBackend : public Backend {
    BMV2Options&        options;
    // P4V1::V1Model&      v1model;
    SumeProgramStructure* structure;

 protected:
    // cstring createCalculation(cstring algo, const IR::Expression* fields,
                              // Util::JsonArray* calculations, bool usePayload, const IR::Node* node);

 public:
    // void modelError(const char* format, const IR::Node* place) const;
    // void convertChecksum(const IR::BlockStatement* body, Util::JsonArray* checksums,
    //                      Util::JsonArray* calculations, bool verify);
    // void createActions(ConversionContext* ctxt, V1ProgramStructure* structure);

    void convert(const IR::ToplevelBlock* tlb) override;
    SimpleSumeSwitchBackend(BMV2Options& options, P4::ReferenceMap* refMap, P4::TypeMap* typeMap,
                        P4::ConvertEnums::EnumMapping* enumMap) :
        Backend(options, refMap, typeMap, enumMap), options(options)/*,
        v1model(P4V1::V1Model::instance)*/ { }
};

}  // namespace BMV2

#endif /* BACKENDS_BMV2_SIMPLE_SWITCH_SIMPLESWITCH_H_ */

