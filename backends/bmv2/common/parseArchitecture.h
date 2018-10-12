namespace BMV2 {

  class ParseArchitecture : public Inspector {
      ProgramStructure* structure;

   public:
      explicit ParseArchitecture(ProgramStructure* structure) :
          structure(structure) { CHECK_NULL(structure); }
      void modelError(const char* format, const IR::Node* node);
      bool preorder(const IR::PackageBlock* block) override;
  };

}
