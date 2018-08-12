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

// #include "frontends/common/model.h"
// #include "simpleSumeSwitch.h"

// namespace BMV2 {

// void SumeProgramStructure::create(ConversionContext* ctxt) {
//     createTypes(ctxt);
//     createHeaders(ctxt);
//     createExterns();
//     createParsers(ctxt);
//     createActions(ctxt);
//     createControls(ctxt);
//     createDeparsers(ctxt);
//     createGlobals();
// }

// void SumeProgramStructure::createStructLike(ConversionContext* ctxt, const IR::Type_StructLike* st) {
//     CHECK_NULL(st);
//     cstring name = st->controlPlaneName();
//     unsigned max_length = 0;  // for variable-sized headers
//     bool varbitFound = false;
//     auto fields = new Util::JsonArray();
//     for (auto f : st->fields) {
//         auto field = new Util::JsonArray();
//         auto ftype = typeMap->getType(f, true);
//         if (ftype->to<IR::Type_StructLike>()) {
//             std::cout << f->name.name << std::endl;
//             BUG("%1%: nested structure", st);
//             // auto e = new IR::Member(fields, f->name);
//             // auto ftype = typeMap->getType(f);
//             // typeMap->setType(e, ftype);
//             // list->push_back(e);
//         } else if (ftype->is<IR::Type_Boolean>()) {
//             field->append(f->name.name);
//             field->append(1);
//             field->append(0);
//             max_length += 1;
//         } else if (auto type = ftype->to<IR::Type_Bits>()) {
//             field->append(f->name.name);
//             field->append(type->size);
//             field->append(type->isSigned);
//             max_length += type->size;
//         } else if (auto type = ftype->to<IR::Type_Varbits>()) {
//             field->append(f->name.name);
//             max_length += type->size;
//             field->append("*");
//             if (varbitFound)
//                 ::error("%1%: headers with multiple varbit fields not supported", st);
//             varbitFound = true;
//         } else if (ftype->is<IR::Type_Error>()) {
//             field->append(f->name.name);
//             field->append(error_width);
//             field->append(0);
//             max_length += error_width;
//         } else if (ftype->to<IR::Type_Stack>()) {
//             BUG("%1%: nested stack", st);
//         } else {
//             BUG("%1%: unexpected type for %2%.%3%", ftype, st, f->name);
//         }
//         fields->append(field);
//     }
//     // must add padding
//     unsigned padding = max_length % 8;
//     if (padding != 0) {
//         cstring name = refMap->newName("_padding");
//         auto field = new Util::JsonArray();
//         field->append(name);
//         field->append(8 - padding);
//         field->append(false);
//         fields->append(field);
//     }

//     unsigned max_length_bytes = (max_length + padding) / 8;
//     if (!varbitFound) {
//         // ignore
//         max_length = 0;
//         max_length_bytes = 0;
//     }
//     ctxt->json->add_header_type(name, fields, max_length_bytes);
// }

// void SumeProgramStructure::createTypes(ConversionContext* ctxt) {
//     for (auto kv : header_types)
//         createStructLike(ctxt, kv.second);
//     for (auto kv : metadata_types)
//         createStructLike(ctxt, kv.second);
//     for (auto kv : header_union_types) {
//         auto st = kv.second;
//         auto fields = new Util::JsonArray();
//         for (auto f : st->fields) {
//             auto field = new Util::JsonArray();
//             auto ftype = typeMap->getType(f, true);
//             auto ht = ftype->to<IR::Type_Header>();
//             CHECK_NULL(ht);
//             field->append(f->name.name);
//             field->append(ht->name.name);
//         }
//         ctxt->json->add_union_type(st->name, fields);
//     }
//     /* TODO */
//     // add errors to json
//     // add enums to json
// }

// void SumeProgramStructure::createHeaders(ConversionContext* ctxt) {
//     for (auto kv : headers) {
//         auto type = kv.second->type->to<IR::Type_StructLike>();
//         ctxt->json->add_header(type->controlPlaneName(), kv.second->name);
//     }
//     for (auto kv : metadata) {
//         auto type = kv.second->type->to<IR::Type_StructLike>();
//         ctxt->json->add_header(type->controlPlaneName(), kv.second->name);
//     }
//     /* TODO */
//     // for (auto kv : header_stacks) {
//     //     json->add_header_stack(stack_type, stack_name, stack_size, ids);
//     // }
//     for (auto kv : header_unions) {
//         auto header_name = kv.first;
//         auto header_type = kv.second->to<IR::Type_StructLike>()->controlPlaneName();
//         // We have to add separately a header instance for all
//         // headers in the union.  Each instance will be named with
//         // a prefix including the union name, e.g., "u.h"
//         Util::JsonArray* fields = new Util::JsonArray();
//         for (auto uf : kv.second->to<IR::Type_HeaderUnion>()->fields) {
//             auto uft = typeMap->getType(uf, true);
//             auto h_name = header_name + "." + uf->controlPlaneName();
//             auto h_type = uft->to<IR::Type_StructLike>()->controlPlaneName();
//             unsigned id = ctxt->json->add_header(h_type, h_name);
//             fields->append(id);
//         }
//         ctxt->json->add_union(header_type, fields, header_name);
//     }
// }

// void SumeProgramStructure::createParsers(ConversionContext* ctxt) {
//     auto cvt = new ParserConverter(ctxt);
//     for (auto kv : parsers) {
//         kv.second->apply(*cvt);
//     }
// }

// void SumeProgramStructure::createExterns() {
//     /* TODO */
//     // add parse_vsets to json
//     // add meter_arrays to json
//     // add counter_arrays to json
//     // add register_arrays to json
//     // add checksums to json
//     // add learn_list to json
//     // add calculations to json
//     // add extern_instances to json
// }

// void SumeProgramStructure::createActions(ConversionContext* ctxt) {
//     auto cvt = new ActionConverter(ctxt, true);
//     for (auto it : actions) {
//         auto action = it.first;
//         action->apply(*cvt);
//     }
// }

// void SumeProgramStructure::createControls(ConversionContext* ctxt) {
//     auto cvt = new BMV2::ControlConverter(ctxt, "TopPipe", true);
//     auto toppipe = pipelines.at("TopPipe");
//     toppipe->apply(*cvt);
// }

// void SumeProgramStructure::createDeparsers(ConversionContext* ctxt) {
//     auto cvt = new DeparserConverter(ctxt);
//     auto ingress = deparsers.at("TopDeparser");
//     ingress->apply(*cvt);
// }

// void SumeProgramStructure::createGlobals() {
//     /* TODO */
//     // for (auto e : globals) {
//     //     convertExternInstances(e->node->to<IR::Declaration>(), e->to<IR::ExternBlock>());
//     // }
// }

// bool ParseSumeArchitecture::preorder(const IR::ToplevelBlock* block) {
//     /// Blocks are not in IR tree, use a custom visitor to traverse
//     for (auto it : block->constantValue) {
//         if (it.second->is<IR::Block>())
//             visit(it.second->getNode());
//     }
//     return false;
// }

// bool ParseSumeArchitecture::preorder(const IR::ExternBlock* block) {
//     if (block->node->is<IR::Declaration>())
//         structure->globals.push_back(block);
//     return false;
// }

// bool ParseSumeArchitecture::preorder(const IR::PackageBlock* block) {
//     // auto pkg = block->getParameterValue("ingress");
//     // if (auto ingress = pkg->to<IR::PackageBlock>()) {
//     //     auto parser = ingress->getParameterValue("ip")->to<IR::ParserBlock>();
//     //     auto pipeline = ingress->getParameterValue("ig")->to<IR::ControlBlock>();
//     //     auto deparser = ingress->getParameterValue("id")->to<IR::ControlBlock>();
//     //     structure->block_type.emplace(parser->container, std::make_pair(INGRESS, PARSER));
//     //     structure->block_type.emplace(pipeline->container, std::make_pair(INGRESS, PIPELINE));
//     //     structure->block_type.emplace(deparser->container, std::make_pair(INGRESS, DEPARSER));
//     // }
//     // pkg = block->getParameterValue("egress");
//     // if (auto egress = pkg->to<IR::PackageBlock>()) {
//     //     auto parser = egress->getParameterValue("ep")->to<IR::ParserBlock>();
//     //     auto pipeline = egress->getParameterValue("eg")->to<IR::ControlBlock>();
//     //     auto deparser = egress->getParameterValue("ed")->to<IR::ControlBlock>();
//     //     structure->block_type.emplace(parser->container, std::make_pair(EGRESS, PARSER));
//     //     structure->block_type.emplace(pipeline->container, std::make_pair(EGRESS, PIPELINE));
//     //     structure->block_type.emplace(deparser->container, std::make_pair(EGRESS, DEPARSER));
//     // }
//     // return false;
// }

// void InspectSumeProgram::postorder(const IR::Declaration_Instance* di) {
//     if (!pinfo->resourceMap.count(di))
//         return;
//     auto blk = pinfo->resourceMap.at(di);
//     if (blk->is<IR::ExternBlock>()) {
//         auto eb = blk->to<IR::ExternBlock>();
//         LOG3("populate " << eb);
//         pinfo->extern_instances.emplace(di->name, di);
//     }
// }

// bool InspectSumeProgram::isHeaders(const IR::Type_StructLike* st) {
//     bool result = false;
//     for (auto f : st->fields) {
//         if (f->type->is<IR::Type_Header>() || f->type->is<IR::Type_Stack>()) {
//             result = true;
//         }
//     }
//     return result;
// }

// void InspectSumeProgram::addHeaderType(const IR::Type_StructLike *st) {
//     if (st->is<IR::Type_HeaderUnion>()) {
//         for (auto f : st->fields) {
//             auto ftype = typeMap->getType(f, true);
//             auto ht = ftype->to<IR::Type_Header>();
//             CHECK_NULL(ht);
//             addHeaderType(ht);
//         }
//         pinfo->header_union_types.emplace(st->getName(), st->to<IR::Type_HeaderUnion>());
//         return;
//     } else if (st->is<IR::Type_Header>()) {
//         pinfo->header_types.emplace(st->getName(), st->to<IR::Type_Header>());
//     } else if (st->is<IR::Type_Struct>()) {
//         pinfo->metadata_types.emplace(st->getName(), st->to<IR::Type_Struct>());
//     }
// }

// void InspectSumeProgram::addHeaderInstance(const IR::Type_StructLike *st, cstring name) {
//     auto inst = new IR::Declaration_Variable(name, st);
//     if (st->is<IR::Type_Header>())
//         pinfo->headers.emplace(name, inst);
//     else if (st->is<IR::Type_Struct>())
//         pinfo->metadata.emplace(name, inst);
//     else if (st->is<IR::Type_HeaderUnion>())
//         pinfo->header_unions.emplace(name, inst);
// }

// void InspectSumeProgram::addTypesAndInstances(const IR::Type_StructLike* type, bool isHeader) {
//     addHeaderType(type);
//     addHeaderInstance(type, type->controlPlaneName());
//     for (auto f : type->fields) {
//         auto ft = typeMap->getType(f, true);
//         if (ft->is<IR::Type_StructLike>()) {
//             // The headers struct can not contain nested structures.
//             if (isHeader && ft->is<IR::Type_Struct>()) {
//                 ::error("Type %1% should only contain headers, header stacks, or header unions",
//                         type);
//                 return;
//             }
//             if (auto hft = ft->to<IR::Type_Header>()) {
//                 addHeaderType(hft);
//                 addHeaderInstance(hft, f->controlPlaneName());
//             } else if (ft->is<IR::Type_HeaderUnion>()) {
//                 for (auto uf : ft->to<IR::Type_HeaderUnion>()->fields) {
//                     auto uft = typeMap->getType(uf, true);
//                     if (auto h_type = uft->to<IR::Type_Header>()) {
//                         addHeaderType(h_type);
//                         addHeaderInstance(h_type, uf->controlPlaneName());
//                     } else {
//                         ::error("Type %1% cannot contain type %2%", ft, uft);
//                         return;
//                     }
//                 }
//                 pinfo->header_union_types.emplace(type->getName(),
//                                                   type->to<IR::Type_HeaderUnion>());
//                 addHeaderInstance(type, f->controlPlaneName());
//             } else {
//                 LOG1("add struct type " << type);
//                 pinfo->metadata_types.emplace(type->getName(), type->to<IR::Type_Struct>());
//                 addHeaderInstance(type, f->controlPlaneName());
//             }
//         } else if (ft->is<IR::Type_Stack>()) {
//             auto stack = ft->to<IR::Type_Stack>();
//             // auto stack_name = f->controlPlaneName();
//             auto stack_size = stack->getSize();
//             auto type = typeMap->getTypeType(stack->elementType, true);
//             BUG_CHECK(type->is<IR::Type_Header>(), "%1% not a header type", stack->elementType);
//             auto ht = type->to<IR::Type_Header>();
//             addHeaderType(ht);
//             auto stack_type = stack->elementType->to<IR::Type_Header>();
//             std::vector<unsigned> ids;
//             for (unsigned i = 0; i < stack_size; i++) {
//                 cstring hdrName = f->controlPlaneName() + "[" + Util::toString(i) + "]";
//                 /* TODO */
//                 // auto id = json->add_header(stack_type, hdrName);
//                 addHeaderInstance(stack_type, hdrName);
//                 // ids.push_back(id);
//             }
//             // addHeaderStackInstance();
//         } else {
//             // Treat this field like a scalar local variable
//             cstring newName = refMap->newName(type->getName() + "." + f->name);
//             if (ft->is<IR::Type_Bits>()) {
//                 auto tb = ft->to<IR::Type_Bits>();
//                 pinfo->scalars_width += tb->size;
//                 pinfo->scalarMetadataFields.emplace(f, newName);
//             } else if (ft->is<IR::Type_Boolean>()) {
//                 pinfo->scalars_width += 1;
//                 pinfo->scalarMetadataFields.emplace(f, newName);
//             } else if (ft->is<IR::Type_Error>()) {
//                 pinfo->scalars_width += 32;
//                 pinfo->scalarMetadataFields.emplace(f, newName);
//             } else {
//                 BUG("%1%: Unhandled type for %2%", ft, f);
//             }
//         }
//     }
// }

// // This visitor only visits the parameter in the statement from architecture.
// bool InspectSumeProgram::preorder(const IR::Parameter* param) {
//     auto ft = typeMap->getType(param->getNode(), true);
//     LOG3("add param " << ft);
//     // only convert parameters that are IR::Type_StructLike
//     if (!ft->is<IR::Type_StructLike>())
//         return false;
//     auto st = ft->to<IR::Type_StructLike>();
//     // parameter must be a type that we have not seen before
//     if (pinfo->hasVisited(st))
//         return false;
//     auto isHeader = isHeaders(st);
//     addTypesAndInstances(st, isHeader);
//     return false;
// }

// void InspectSumeProgram::postorder(const IR::P4Parser* p) {
//     if (pinfo->block_type.count(p)) {
//         auto info = pinfo->block_type.at(p);
//         if (info.first == INGRESS && info.second == PARSER)
//             pinfo->parsers.emplace("ingress", p);
//         else if (info.first == EGRESS && info.second == PARSER)
//             pinfo->parsers.emplace("egress", p);
//     }
// }

// void InspectSumeProgram::postorder(const IR::P4Control *c) {
//     if (pinfo->block_type.count(c)) {
//         auto info = pinfo->block_type.at(c);
//         if (info.first == INGRESS && info.second == PIPELINE)
//             pinfo->pipelines.emplace("ingress", c);
//         else if (info.first == EGRESS && info.second == PIPELINE)
//             pinfo->pipelines.emplace("egress", c);
//         else if (info.first == INGRESS && info.second == DEPARSER)
//             pinfo->deparsers.emplace("ingress", c);
//         else if (info.first == EGRESS && info.second == DEPARSER)
//             pinfo->deparsers.emplace("egress", c);
//     }
// }

// void SimpleSumeSwitchBackend::convert(const IR::ToplevelBlock* tlb) {
//     CHECK_NULL(tlb);
//     SumeProgramStructure structure(refMap, typeMap);

//     auto parseSumeArch = new ParseSumeArchitecture(&structure);
//     auto main = tlb->getMain();
//     if (!main) return;
//     main->apply(*parseSumeArch);

//     auto evaluator = new P4::EvaluatorPass(refMap, typeMap);
//     auto program = tlb->getProgram();
//     PassManager simplify = {
//         /* TODO */
//         // new RenameUserMetadata(refMap, userMetaType, userMetaName),
//         new P4::ClearTypeMap(typeMap),  // because the user metadata type has changed
//         // new P4::SynthesizeActions(refMap, typeMap, new SkipControls(&non_pipeline_controls)),
//         new P4::MoveActionsToTables(refMap, typeMap),
//         new P4::TypeChecking(refMap, typeMap),
//         new P4::SimplifyControlFlow(refMap, typeMap),
//         new LowerExpressions(typeMap),
//         new P4::ConstantFolding(refMap, typeMap, false),
//         new P4::TypeChecking(refMap, typeMap),
//         // new RemoveComplexExpressions(refMap, typeMap, new ProcessControls(&pipeline_controls)),
//         new P4::SimplifyControlFlow(refMap, typeMap),
//         new P4::RemoveAllUnusedDeclarations(refMap),
//         evaluator,
//         new VisitFunctor([this, evaluator, structure]() {
//             toplevel = evaluator->getToplevelBlock(); }),
//     };
//     program->apply(simplify);

//     // map IR node to compile-time allocated resource blocks.
//     toplevel->apply(*new BMV2::BuildResourceMap(&structure.resourceMap));

//     PassManager toJson = {
//         new DiscoverStructure(&structure),
//         new InspectSumeProgram(refMap, typeMap, &structure),
//         new ConvertSumeToJson(refMap, typeMap, toplevel, json, &structure)
//     };
//     program->apply(toJson);

//     json->add_program_info(options.file);
//     json->add_meta_info();
// }

// }  // namespace BMV2

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

bool ParseSumeArchitecture::preorder(const IR::PackageBlock* main) {
    std::cout << "in preorder!" << std::endl;
    main->dbprint(std::cout);
    auto prsr = main->findParameterValue("p");
    if (prsr == nullptr || !prsr->is<IR::ParserBlock>()) {
        // modelError("%1%: main package  match the expected model", main);
        return false;
    }
    structure->block_type.emplace(prsr->to<IR::ParserBlock>()->container, PARSER);
    std::cout << "parse block size: " << structure->block_type.size() << std::endl;

    auto ingress = main->findParameterValue("map");
    if (ingress == nullptr || !ingress->is<IR::ControlBlock>()) {
        // modelError("%1%: main package does not match the expected model", main);
        return false;
    }
    auto ingress_name = ingress->to<IR::ControlBlock>()->container->name;
    structure->block_type.emplace(ingress->to<IR::ControlBlock>()->container, PIPELINE);
    structure->pipeline_controls.emplace(ingress_name);
    std::cout << "pipe block size: " << structure->block_type.size() << std::endl;

    // auto verify = main->findParameterValue(v1model.sw.verify.name);
    // if (verify == nullptr || !verify->is<IR::ControlBlock>()) {
    //     modelError("%1%: main package does not match the expected model", main);
    //     return false;
    // }
    // structure->block_type.emplace(verify->to<IR::ControlBlock>()->container, V1_VERIFY);
    // structure->non_pipeline_controls.emplace(verify->to<IR::ControlBlock>()->container->name);

    // auto egress = main->findParameterValue(v1model.sw.egress.name);
    // if (egress == nullptr || !egress->is<IR::ControlBlock>()) {
    //     modelError("%1%: main package does not match the expected model", main);
    //     return false;
    // }
    // auto egress_name = egress->to<IR::ControlBlock>()->container->name;
    // structure->block_type.emplace(egress->to<IR::ControlBlock>()->container, V1_EGRESS);
    // structure->pipeline_controls.emplace(egress_name);

    // auto compute = main->findParameterValue(v1model.sw.compute.name);
    // if (compute == nullptr || !compute->is<IR::ControlBlock>()) {
    //     modelError("%1%: main package does not match the expected model", main);
    //     return false;
    // }
    // structure->block_type.emplace(compute->to<IR::ControlBlock>()->container, V1_COMPUTE);
    // structure->non_pipeline_controls.emplace(compute->to<IR::ControlBlock>()->container->name);

    // TODO: sume model like v1model
    auto deparser = main->findParameterValue("d");
    if (deparser == nullptr || !deparser->is<IR::ControlBlock>()) {
        // modelError("%1%: main package  match the expected model", main);
        return false;
    }
    structure->block_type.emplace(deparser->to<IR::ControlBlock>()->container, DEPARSER);
    structure->non_pipeline_controls.emplace(deparser->to<IR::ControlBlock>()->container->name);

    return false;
}

// void
// SimpleSwitchBackend::modelError(const char* format, const IR::Node* node) const {
//     ::error(format, node);
//     ::error("Are you using an up-to-date v1model.p4?");
// }

// cstring
// SimpleSwitchBackend::createCalculation(cstring algo, const IR::Expression* fields,
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
// SimpleSwitchBackend::convertChecksum(const IR::BlockStatement *block, Util::JsonArray* checksums,
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

// void SimpleSumeSwitchBackend::createActions(ConversionContext* ctxt, V1ProgramStructure* structure) {
//     auto cvt = new ActionConverter(ctxt, options.emitExterns);
//     for (auto it : structure->actions) {
//         auto action = it.first;
//         action->apply(*cvt);
//     }
// }

void
SimpleSumeSwitchBackend::convert(const IR::ToplevelBlock* tlb) {
    // std::cout << "starting conversion" <<std::endl;
    structure = new SumeProgramStructure();
    // std::cout << "1" <<std::endl;

    auto parseSumeArch = new ParseSumeArchitecture(structure);
    // std::cout << "2" <<std::endl;
    auto main = tlb->getMain();
    // std::cout << "3" <<std::endl;
    if (!main) {
        std::cout << "no main 0" << std::endl;
        return;  // no main
    }
    // std::cout << "4" <<std::endl;
    main->apply(*parseSumeArch);
    // std::cout << "5" <<std::endl;
    if (::errorCount() > 0) {
        std::cout << "error count > 0" << std::endl;
        return;
    }
    // std::cout << "6" <<std::endl;

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
    // std::cout << "7" <<std::endl;
    auto program = tlb->getProgram();
    // std::cout << "8" <<std::endl;
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
    // std::cout << "9" <<std::endl;

    auto hook = options.getDebugHook();
    // std::cout << "10" <<std::endl;
    simplify.addDebugHook(hook);
    // std::cout << "11" <<std::endl;

    program->apply(simplify);
    // std::cout << "12" <<std::endl;

    // map IR node to compile-time allocated resource blocks.
    toplevel->apply(*new BMV2::BuildResourceMap(&structure->resourceMap));
    // std::cout << "13" <<std::endl;

    // field list and learn list ids in bmv2 are not consistent with ids for
    // other objects: they need to start at 1 (not 0) since the id is also used
    // as a "flag" to indicate that a certain simple_switch primitive has been
    // called (e.g. resubmit or generate_digest)
    BMV2::nextId("field_lists");
    BMV2::nextId("learn_lists");
    json->add_program_info(options.file);
    json->add_meta_info();

    // std::cout << "14" <<std::endl;

    // convert all enums to json
    for (const auto &pEnum : *enumMap) {
        auto name = pEnum.first->getName();
        for (const auto &pEntry : *pEnum.second) {
            json->add_enum(name, pEntry.first, pEntry.second);
        }
    }

    // std::cout << "15" <<std::endl;

    if (::errorCount() > 0) {
        std::cout << "error count > 0" << std::endl;
        return;
    }

    // std::cout << "16" <<std::endl;

    /// generate error types
    for (const auto &p : structure->errorCodesMap) {
        auto name = p.first->toString();
        auto type = p.second;
        json->add_error(name, type);
    }

    // std::cout << "17" <<std::endl;

    main = toplevel->getMain();
    if (!main) {
        std::cout << "no main" << std::endl;
        return;  // no main
    }

    // std::cout << "18" <<std::endl;

    main->apply(*parseSumeArch);
    PassManager updateStructure {
        new DiscoverSumeStructure(structure),
    };
    program = toplevel->getProgram();
    program->apply(updateStructure);

    // std::cout << "19" <<std::endl;

    cstring scalarsName = refMap->newName("scalars");
    // This visitor is used in multiple passes to convert expression to json
    conv = new SimpleSumeSwitchExpressionConverter(refMap, typeMap, structure, scalarsName);

    // std::cout << "20" <<std::endl;

    auto ctxt = new ConversionContext(refMap, typeMap, toplevel, structure, conv, json);

    auto hconv = new HeaderConverter(ctxt, scalarsName);
    program->apply(*hconv);

    // std::cout << "21" <<std::endl;

    auto pconv = new ParserConverter(ctxt);
    // std::cout << "21.1" <<std::endl;
    std::cout << structure->parser << std::endl;
    structure->parser->dbprint(std::cout);
    // std::cout << "21.2" <<std::endl;
    structure->parser->apply(*pconv);

    std::cout << "22" <<std::endl;

    // createActions(ctxt, structure);

    auto cconv = new ControlConverter(ctxt, "TopPipe", options.emitExterns);
    std::cout << "23" <<std::endl;
    structure->ingress->apply(*cconv);
    std::cout << "24" <<std::endl;

    auto dconv = new DeparserConverter(ctxt);
    std::cout << "25" <<std::endl;
    structure->deparser->apply(*dconv);
    std::cout << "26" <<std::endl;

    // convertChecksum(structure->compute_checksum->body, json->checksums,
    //                 json->calculations, false);

    // convertChecksum(structure->verify_checksum->body, json->checksums,
    //                 json->calculations, true);

    // (void)toplevel->apply(ConvertGlobals(ctxt, options.emitExterns));
}

}  // namespace BMV2

