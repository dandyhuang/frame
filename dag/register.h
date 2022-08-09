#pragma once
#include <iostream>
#include <map>
#include <memory>
namespace dag {
class Node;
class ServiceFactory {
 public:
  virtual Node* create() = 0;
};
class NodeFactoryMap {
 public:
  std::map<std::string, ServiceFactory*> global_node_map;
  static std::shared_ptr<NodeFactoryMap> Instance();

 private:
  NodeFactoryMap() = default;
};
}  // namespace dag

#define SERVICE_REGISTER(ServiceClass)                          \
  class ServiceClass##Factory : public ::dag::ServiceFactory {  \
   public:                                                      \
    dag::Node* create() override { return new ServiceClass(); } \
  };                                                            \
  class ServiceClass##Register {                                \
   public:                                                      \
    ServiceClass##Register() {                                  \
      dag::NodeFactoryMap::Instance()->global_node_map.insert( \
          {#ServiceClass, new ServiceClass##Factory()});        \
    }                                                           \
  };                                                            \
  static ServiceClass##Register ServiceClass##_inst_;

#define GET_SERVICE_FACTORY(ServiceClassName) \
  dag::NodeFactoryMap::Instance()->global_node_map[ServiceClassName];
