#ifndef BACKENDS_BMV2_COMMON_FRONTEND_H_
#define BACKENDS_BMV2_COMMON_FRONTEND_H_

namespace BMV2 {

template<class BackendImpl> // Should be subclass of type Backend
class FrontEnd {
  public:
    explicit FrontEnd() {};

    int run(int argc, char *const argv[]);
};

}  // namespace BMV2

#endif /* BACKENDS_BMV2_COMMON_FRONTEND_H_ */
