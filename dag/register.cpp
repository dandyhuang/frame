#include "dag/register.h"

namespace dag {

std::shared_ptr<NodeFactoryMap> NodeFactoryMap::Instance() {
  static std::shared_ptr<NodeFactoryMap> handle =
      std::shared_ptr<NodeFactoryMap>(new NodeFactoryMap());
  return handle;
}
}  // namespace dag
