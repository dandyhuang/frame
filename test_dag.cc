#include <iostream>

#include "common/loghelper.h"
#include "dag/graph.h"
#include "dag/graph_manager.h"
#include "dag/node_manager.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "proto/faiss_search.pb.h"

DEFINE_string(gflags_config, "conf/gflags.conf", "gflags conf");
int main(int argc, char const* argv[]) {
  google::AllowCommandLineReparsing();
  google::ParseCommandLineFlags(&argc, const_cast<char***>(&argv), true);
  google::SetCommandLineOption("flagfile", FLAGS_gflags_config.c_str());
  //  使用monitor error打印监控日
  vlog::global_vlog_helper().setAppLogLevel(0);

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
    graph->run<faiss::FaissRequest, faiss::FaissResponse>(nullptr, &req, &rsp, done);
  } else {
    if (done != nullptr) done->Run();
  }
  return 0;
}