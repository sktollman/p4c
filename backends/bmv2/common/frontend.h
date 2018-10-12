#ifndef BACKENDS_BMV2_COMMON_FRONTEND_H_
#define BACKENDS_BMV2_COMMON_FRONTEND_H_

#include "backends/bmv2/simple_switch/simpleSwitch.h"

namespace BMV2 {

template<class BackendImpl> // Should be subclass of type Backend
class FrontEnd {
  public:
    explicit FrontEnd<BackendImpl>() {};

    int run(int argc, char *const argv[]);
};

template class FrontEnd<SimpleSwitchBackend>;

}  // namespace BMV2

#endif /* BACKENDS_BMV2_COMMON_FRONTEND_H_ */
