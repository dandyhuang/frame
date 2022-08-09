#include <iostream>

#include "dag/graph.h"
#include "dag/graph_manager.h"
#include "dag/node_manager.h"
#include "proto/faiss_search.pb.h"

int main() {
  std::string node_path = "./conf/node.xml";
  dag::common::NodeManager::Instance().InitNodeConf(node_path);
  std::string graph_path = "./conf/graph.xml";
  dag::common::GraphManager::Instance().InitGraphConf(graph_path);

  std::string graph_name = "default";
  google::protobuf::Closure* done;
  // brpc::ClosureGuard done_guard(done);
  faiss::FaissRequest req;
  faiss::FaissResponse rsp;
  auto graph = ::dag::common::GraphManager::Instance().get_graph(graph_name);
  if (graph) {
    graph->run<faiss::FaissRequest, faiss::FaissResponse>(cntl, &req, &rsp, done);
  } else {
    if (done != nullptr) done->Run();
  }
  return 0;
}