#pragma once

#include <bvar/bvar.h>
#include <glog/logging.h>
#include <stdint.h>
#include <time.h>

#include <atomic>

#include "mixer_common/loghelper.h"

namespace rec {
namespace common {

class InterfaceLimiter {
 public:
  InterfaceLimiter() {}
  virtual ~InterfaceLimiter() {}

  virtual bool Allow(int take = 1) = 0;
  virtual float GetQps() {};
  virtual void UpdateStatus() {}
};

}  // namespace common
}  // namespace rec
