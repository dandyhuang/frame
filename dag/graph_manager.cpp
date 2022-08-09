#include "dag/graph_manager.h"

#include <boost/algorithm/string.hpp>

#include "dag/graph.h"
#include "dag/node_manager.h"
namespace dag {
namespace common {
using dag::Graph;
using dag::Node;
void GraphManager::InitGraphConf(const std::string& file_path) {
  common::ConfigXml xml_conf;
  xml_conf.init(file_path);
  common::ConfigXml main_config;
  if (!xml_conf.Find("main", main_config)) {
    VLOG_APP(ERROR) << "main not found in " << file_path;
    return;
  }
  bool has_next = false;

  std::string main_name;
  main_config.Attr<std::string>("name", main_name);
  common::ConfigXml graph;
  if (!main_config.Child("graph", graph)) {
    main_config << file_path << "graph node not exist in file";
    return;
  }
  do {
    std::string name;
    graph.Attr<std::string>("name", name);
    auto graph_ptr = CreateGraph(graph);
    if (graph_ptr == nullptr) {
      VLOG_APP(ERROR) << "[" << name << "] ERROR_FRAME_GRAPH_CONF_PARSE_FAIL";
      return;
    }

    graph_map_.insert({name, graph});
    has_next = graph.Next("graph", graph);
  } while (has_next);
}

std::shared_ptr<dag::Graph> CreateGraph(const common::ConfigXml& graph_conf) {
  std::string graph_name;
  graph_conf.Attr<std::string>("name", graph_name);
  std::shared_ptr<vv_feed::ConfigXml> node_conf_ptr = std::make_shared<vv_feed::ConfigXml>();
  auto& node_conf = *node_conf_ptr;
  if (!graph_conf.Child("node", node_conf)) {
    VLOG_APP(ERROR) << "graph_node not exist in file: " << file_path;
    return;
  }
  std::shared_ptr<dag::Graph> graph = std::make_shared<dag::Graph>();
  dag::Node* root = nullptr;
  std::unordered_map<std::string, Node*> nodes_map;
  auto create_service_if_not_exist = [&nodes_map](const std::string name) -> dag::Node* {
    // 会有很多重复的node
    auto it = nodes_map.find(name);
    if (it != nodes_map.end()) {
      return it->second;
    }
    auto node_config = NodeManager::Instance().GetNodeInfo(name);
    if (!node_config) {
      VLOG_APP(ERROR) << "node name: " << name << " not found in node manager";
      std::cout << "node name: " << name << " not found in node manager" << std::endl;
      return nullptr;
    }

    std::string clazz;
    node_config->Attr<std::string>("class", clazz);
    auto service_fac = GET_SERVICE_FACTORY(clazz);
    if (!service_fac) {
      VLOG_APP(ERROR) << "unregistered class: " << clazz;
      std::cout << "unregistered class: " << clazz << std::endl;
      return nullptr;
    }
    auto service_node = service_fac->create();
    // 初始化每个node节点
    service_node->init(node_config);
    service_node->set_name(name);
    nodes_map[name] = service_node;
    return service_node;
  };
  bool has_next = false;
  do {
    std::string name;
    node_conf.Attr<std::string>("name", name);
    Node* service = create_service_if_not_exist(name);
    if (!service) {
      VLOG_APP(ERROR) << "graph node name: " << name << " create failed";
      std::cout << "graph node name: " << name << " create failed" << std::endl;
      has_next = node_conf.Next("node", node_conf);
      continue;
    }
    if (root == nullptr) {
      // first node is root by default
      root = service;
    }
    {
      std::string children = "";
      node_conf.Attr<std::string>("children", children);
      if (!children.empty()) {
        boost::trim(children);
        std::vector<std::string> child_vec;
        boost::split(child_vec, children, boost::is_any_of(","));
        for (std::string& c : child_vec) {
          boost::trim(c);
          dag::Node* child = create_service_if_not_exist(c);
          if (!child) {
            continue;
          }
          service->add_output(child);
        }
      }
    }
    {
      std::string parent = "";
      node_conf.Attr<std::string>("parent", parent);
      if (!parent.empty()) {
        boost::trim(parent);
        std::vector<std::string> p_vec;
        boost::split(p_vec, parent, boost::is_any_of(","));
        for (std::string& p : p_vec) {
          boost::trim(p);
          dag::Node* p_node = create_service_if_not_exist(p);
          if (!p_node) {
            continue;
          }
          p_node->add_output(service);
        }
      }
    }
    has_next = node_conf.Next("node", node_conf);
  } while (has_next);
  graph->init(root, graph_name);
  return graph;
}

std::shared_ptr<dag::Graph> GraphManager::get_graph(const std::string& main_name) {
  auto it = graph_map_.find(main_name);
  if (it == graph_map_.end()) {
    return std::shared_ptr<Graph>();
  } else {
    return it->second;
  }
}
}  // namespace common
}  // namespace dag
