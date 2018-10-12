
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

typedef struct {
  string name;
  string label;
} arch;

arch parser;
arch[] controlBlocks;
arch 


bool ParseV1Architecture::preorder(const IR::PackageBlock* main) {
    auto prsr = main->findParameterValue(v1model.sw.parser.name);
    if (prsr == nullptr || !prsr->is<IR::ParserBlock>()) {
        modelError("%1%: main package  match the expected model", main);
        return false;
    }
    structure->block_type.emplace(prsr->to<IR::ParserBlock>()->container, V1_PARSER);

    auto ingress = main->findParameterValue(v1model.sw.ingress.name);
    if (ingress == nullptr || !ingress->is<IR::ControlBlock>()) {
        modelError("%1%: main package does not match the expected model", main);
        return false;
    }
    auto ingress_name = ingress->to<IR::ControlBlock>()->container->name;
    structure->block_type.emplace(ingress->to<IR::ControlBlock>()->container, V1_INGRESS);
    structure->pipeline_controls.emplace(ingress_name);

    auto verify = main->findParameterValue(v1model.sw.verify.name);
    if (verify == nullptr || !verify->is<IR::ControlBlock>()) {
        modelError("%1%: main package does not match the expected model", main);
        return false;
    }
    structure->block_type.emplace(verify->to<IR::ControlBlock>()->container, V1_VERIFY);
    structure->non_pipeline_controls.emplace(verify->to<IR::ControlBlock>()->container->name);

    auto egress = main->findParameterValue(v1model.sw.egress.name);
    if (egress == nullptr || !egress->is<IR::ControlBlock>()) {
        modelError("%1%: main package does not match the expected model", main);
        return false;
    }
    auto egress_name = egress->to<IR::ControlBlock>()->container->name;
    structure->block_type.emplace(egress->to<IR::ControlBlock>()->container, V1_EGRESS);
    structure->pipeline_controls.emplace(egress_name);

    auto compute = main->findParameterValue(v1model.sw.compute.name);
    if (compute == nullptr || !compute->is<IR::ControlBlock>()) {
        modelError("%1%: main package does not match the expected model", main);
        return false;
    }
    structure->block_type.emplace(compute->to<IR::ControlBlock>()->container, V1_COMPUTE);
    structure->non_pipeline_controls.emplace(compute->to<IR::ControlBlock>()->container->name);

    auto deparser = main->findParameterValue(v1model.sw.deparser.name);
    if (deparser == nullptr || !deparser->is<IR::ControlBlock>()) {
        modelError("%1%: main package  match the expected model", main);
        return false;
    }
    structure->block_type.emplace(deparser->to<IR::ControlBlock>()->container, V1_DEPARSER);
    structure->non_pipeline_controls.emplace(deparser->to<IR::ControlBlock>()->container->name);

    return false;
}
