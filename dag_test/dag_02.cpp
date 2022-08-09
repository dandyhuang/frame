#include "dag_test/dag_02.h"

#include <string>
#include <vector>

#include "dag/register.h"

namespace dag {
SERVICE_REGISTER(dag_002)
int dag_002::do_service(std::shared_ptr<frame::Context> context) noexcept {
   for (auto i = 0; i < 100000; i++) {
      if (i % 1000 == 0) {
         usleep(10);
      }
   }
   std::cout << "dag_002" << std::endl;
}

bool dag_002::skip(std::shared_ptr<frame::Context> context) { return false; }

}  // namespace dag