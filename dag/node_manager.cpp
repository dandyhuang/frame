#include "dag/node_manager.h"

#include <iostream>
#include <string>

#include "client/redis_client_manager.h"
#include "dag/gtransport_manager.h"
// #include "dag/global.h"

namespace dag {
namespace common {
void NodeManager::InitNodeConf(const std::string& file_path) {
  static std::unordered_map<std::string, std::shared_ptr<::common::ConfigXml>> xml_conf_map;
  std::shared_ptr<::common::ConfigXml> xml_conf_ptr = std::make_shared<::common::ConfigXml>();
  xml_conf_map.insert({file_path, xml_conf_ptr});

  auto& xml_conf = *xml_conf_ptr;
  xml_conf.init(file_path);
  ::common::ConfigXml nodes_config;
  if (!xml_conf.Find("nodes_config", nodes_config)) {
    VLOG_APP(ERROR) << "nodes_config not found in " << file_path;
    return;
  }
  std::shared_ptr<::common::ConfigXml> node_conf_ptr = std::make_shared<::common::ConfigXml>();
  if (!nodes_config.Child("node", *node_conf_ptr)) {
    VLOG_APP(ERROR) << "nodes_config not exist in file: " << file_path;
    return;
  }
  bool has_next = false;
  do {
    std::string name;
    node_conf_ptr->Attr<std::string>("name", name);
    VLOG_APP(ERROR) << "load node name: " << name << std::endl;
    std::string clazz;
    node_conf_ptr->Attr<std::string>("class", clazz);
    std::string transport;
    node_conf_ptr->Attr<std::string>("transport_name", transport);
    auto it = nodes_map_.find(name);
    if (it != nodes_map_.end()) {
      VLOG_APP(ERROR) << "node: " << name << " already exists";
    } else {
      nodes_map_[name] = node_conf_ptr;
    }
    // if (!transport.empty()) {
    //   if (!GtransportManager::instance().get_transport(transport) &&
    //       !client::RedisClientManager::instance()->get_client(transport)) {
    //     VLOG_APP(ERROR) << "conf error !! transport_name:" << transport
    //                     << " not exist in gtransport and redis client";
    //     std::cout << "conf error !! transport_name:" << transport
    //               << " not exist in gtransport and redis client" << std::endl;
    //   }
    // }
    auto next_node_conf_ptr = std::make_shared<::common::ConfigXml>();
    has_next = node_conf_ptr->Next("node", *next_node_conf_ptr);
    node_conf_ptr = std::move(next_node_conf_ptr);

  } while (has_next);
  VLOG_APP(ERROR) << "nodes_map_ size: " << nodes_map_.size() << std::endl;
  std::cout << "nodes_map_ size: " << nodes_map_.size() << std::endl;
}
}  // end of namespace common
}  // end of namespace dag
