#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>

#include "common/config_xml.h"
#include "common/loghelper.h"

namespace dag {
namespace common {
using namespace vlog;
class NodeManager {
 private:
  NodeManager(){};
  std::unordered_map<std::string, std::shared_ptr<dag::ConfigXml>> nodes_map;

 public:
  static NodeManager& instance() {
    static NodeManager inst;
    return inst;
  }

  void AddNodesConf(const std::string& file_path);
  std::shared_ptr<dag::ConfigXml> GetNodeInfo(const std::string& node_name) {
    auto it = nodes_map.find(node_name);
    if (it != nodes_map.end()) {
      return it->second;
    } else {
      std::cout << "node_name: " << node_name << " found in nodes_map" << std::endl;
      return std::shared_ptr<dag::ConfigXml>();
    }
  }
};
}  // end of namespace common
}  // end of namespace dag
