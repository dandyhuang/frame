#include "dag_test/dag_01.h"

#include <string>
#include <vector>

#include "dag/register.h"

namespace dag {
SERVICE_REGISTER(dag_001)
int dag_001::do_service(std::shared_ptr<frame::Context> context) noexcept {
   for (auto i = 0; i < 100000; i++) {
      if (i % 1000 == 0) {
         usleep(10);
      }
   }
}

bool dag_001::skip(std::shared_ptr<frame::Context> context) { return false; }

}  // namespace dag