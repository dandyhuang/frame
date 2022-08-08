#pragma once
#include <string>
#include <unordered_map>
#include <memory>
namespace dag {
class Graph;
class Node;
} // end of namespace dag
namespace dag {
namespace common {
class GraphManager {
private:
    GraphManager(){}
public:
    static GraphManager& instance() {
        static GraphManager inst;
        return inst;
    }

    void add_graph_conf(const std::string& conf_path);
    std::shared_ptr<dag::Graph> get_graph(const std::string& topo_name);
private:
    dag::Node* create_service(const std::string name);
    std::unordered_map<std::string, std::shared_ptr<dag::Graph>> graph_map;
};
} // end of namespace
} // end of namespace
