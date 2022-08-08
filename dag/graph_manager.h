#pragma once
#include <memory>
#include <string>
#include <unordered_map>
namespace dag {
class Graph;
class Node;
}  // end of namespace dag
namespace dag {
namespace common {
class GraphManager {
 private:
  GraphManager() {}

 public:
  static GraphManager& instance() {
    static GraphManager inst;
    return inst;
  }

  void InitGraphConf(const std::string& conf_path);
  std::shared_ptr<dag::Graph> get_graph(const std::string& topo_name);

 private:
  dag::Node* create_service(const std::string name);
  void CreateGraph();
  std::unordered_map<std::string, std::shared_ptr<dag::Graph>> graph_map_;
};
}  // namespace common
}  // namespace dag
