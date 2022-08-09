#include "dag_test/dag_00.h"

#include <string>
#include <vector>

#include "dag/register.h"

namespace dag {
SERVICE_REGISTER(dag_000)
int dag_000::do_service(std::shared_ptr<frame::Context> context) noexcept {
   for (auto i = 0; i < 100000; i++) {
      if (i % 1000 == 0) {
         usleep(10);
      }
   }
   std::cout << "dag_000" << std::endl;
}

bool dag_000::skip(std::shared_ptr<frame::Context> context) { return false; }

}  // namespace dag