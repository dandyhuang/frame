#include <iostream>

#include "dag/graph.h"
#include "dag/graph_manager.h"
#include "dag/node_manager.h"
int main() {
  std::string node_path = "./conf/node.xml";
  dag::common::NodeManager::Instance().InitNodeConf(node_path);
  std::string graph_path = "./conf/graph.xml";
  dag::common::GraphManager::Instance().InitGraphConf(graph_path);

  std::string graph_name = "default";
  if (!request->graph_name().empty()) {
    graph_name = request->graph_name();
  }
  auto graph = ::dag::common::GraphManager::Instance().get_graph(graph_name);
  if (graph) {
    graph->run<zeus::proto::ZeusRequest, zeus::proto::ZeusResponse>(cntl, request, response, done);
  } else {
    done->Run();
  }
  return 0;
}