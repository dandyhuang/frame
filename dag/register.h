#pragma once
#include <map>
#include <memory>
#include <iostream>
namespace dag {
 class Node;
        class ServiceFactory {
        public:
            virtual Node* create() = 0;
        };
        class NodeFactory_map{
        public:
            std::map<std::string, ServiceFactory*> global_node_map;
            static std::shared_ptr<NodeFactory_map> instance();
        private:
            NodeFactory_map() = default;
        };
} // end of namespace

#define SERVICE_REGISTER(ServiceClass) class ServiceClass##Factory : public ::dag::ServiceFactory { \
public: \
dag::Node* create() override { \
    return new ServiceClass(); \
} \
}; \
class ServiceClass##Register { \
public: \
ServiceClass##Register() { \
    dag::NodeFactory_map::instance()->global_node_map.insert({#ServiceClass, new ServiceClass##Factory()}); \
} \
}; \
static ServiceClass##Register ServiceClass##_inst_;

#define GET_SERVICE_FACTORY(ServiceClassName) dag::NodeFactory_map::instance()->global_node_map[ServiceClassName];
