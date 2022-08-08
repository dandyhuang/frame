#include "dag/register.h"

namespace dag {

std::shared_ptr<NodeFactory_map> NodeFactory_map::instance()
{
    static std::shared_ptr<NodeFactory_map> handle = std::shared_ptr<NodeFactory_map>(new NodeFactory_map());
    return handle;
}
} // end of namespace
