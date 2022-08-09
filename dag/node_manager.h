#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>

#include "common/conf/config_xml.h"
#include "common/loghelper.h"

namespace dag {
namespace common {
using namespace vlog;
class NodeManager {
 private:
  NodeManager(){};
  std::unordered_map<std::string, std::shared_ptr<::common::ConfigXml>> nodes_map_;

 public:
  static NodeManager& Instance() {
    static NodeManager inst;
    return inst;
  }

  void InitNodeConf(const std::string& file_path);
  std::shared_ptr<::common::ConfigXml> GetNodeInfo(const std::string& node_name) {
    auto it = nodes_map_.find(node_name);
    if (it != nodes_map_.end()) {
      return it->second;
    } else {
      std::cout << "node_name: " << node_name << " found in nodes_map_" << std::endl;
      return std::shared_ptr<::common::ConfigXml>();
    }
  }
};
}  // end of namespace common
}  // end of namespace dag
