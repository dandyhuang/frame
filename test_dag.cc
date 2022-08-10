#include <iostream>

#include "common/loghelper.h"
#include "dag/graph.h"
#include "dag/graph_manager.h"
#include "dag/node_manager.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "proto/faiss_search.pb.h"
std::vector<bthread_t> g_test_bt_vec;
std::vector<std::shared_ptr<std::future<void*>>> g_testfuture_res;

DEFINE_string(gflags_config, "conf/gflags.conf", "gflags conf");
int main(int argc, char const* argv[]) {
  google::AllowCommandLineReparsing();
  google::ParseCommandLineFlags(&argc, const_cast<char***>(&argv), true);
  google::SetCommandLineOption("flagfile", FLAGS_gflags_config.c_str());
  //  使用monitor error打印监控日
  vlog::global_vlog_helper().initialize(argc, const_cast<char**>(argv));
  vlog::global_vlog_helper().setMonitorLogLevel(0);
  vlog::global_vlog_helper().setAppLogLevel(0);
  vlog::global_vlog_helper().setSysLogLevel(0);
  vlog::global_vlog_helper().setModelLogLevel(0);

  std::string node_path = "./conf/node.xml";
  dag::common::NodeManager::Instance().InitNodeConf(node_path);
  std::string graph_path = "./conf/graph.xml";
  dag::common::GraphManager::Instance().InitGraphConf(graph_path);

  std::string graph_name = "default";
  google::protobuf::Closure* done;
  // brpc::ClosureGuard done_guard(done);
  faiss::FaissRequest req;
  faiss::FaissResponse rsp;
  int64_t start = butil::gettimeofday_us();
  for (auto i = 0; i < 1; i++) {
    auto graph = ::dag::common::GraphManager::Instance().get_graph(graph_name);
    if (graph) {
      graph->run<faiss::FaissRequest, faiss::FaissResponse>(
          nullptr, &req, &rsp, done);
    } else {
      if (done != nullptr) done->Run();
    }
  }
#ifdef DAG_SYNCHRONIZE_USE

#ifdef DAG_THREAD_USE
  for (auto&& result : g_testfuture_res) {
    auto res = result.get();  // wait
  }
#else
  for (auto& b : g_test_bt_vec) {
    bthread_join(b, nullptr);
  }
#endif
#endif

  std::cout << "cost:" << butil::gettimeofday_us() - start << std::endl;
  sleep(1);
  return 0;
}