#pragma once
#include <brpc/channel.h>

#include <atomic>
#include <functional>
#include <memory>
#include <queue>
#include <set>
#include <unordered_map>
#include <vector>

#include "brpc/callback.h"
#include "bthread/bthread.h"
#include "bthread/id.h"
#include "common/conf/config_xml.h"
#include "common/loghelper.h"
#include "context/context.h"
#include "dag/graph_context.h"
#include "dag/register.h"

namespace dag {
class Context;
}  // namespace dag
namespace dag {
using namespace vlog;
class Node;
struct Bargs {
  Bargs(Node* n, std::shared_ptr<frame::Context> ctx) : node(n), context(ctx) {}
  Node* node;
  std::shared_ptr<frame::Context> context;
};
class Node {
 public:
  Node(const std::string& name = "") : name(name) {}
  virtual void init(std::shared_ptr<::common::ConfigXml>);
  void run(std::shared_ptr<frame::Context> context);
  virtual int do_service(std::shared_ptr<frame::Context> context) noexcept;
  virtual bool skip(std::shared_ptr<frame::Context> context);
  bool notify(std::shared_ptr<frame::Context> context);

  const std::vector<Node*>& get_output_nodes() { return out_nodes; }
  void set_name(const std::string& name) {
    this->name = name;
    recorder = std::make_shared<bvar::LatencyRecorder>("predictor_" + name + "latency");
  }

  const std::string& get_name() { return name; }

  void add_output(Node* node) {
    out_nodes.push_back(node);
    node->incr_input_num();
  }
  size_t get_out_nodes_size() { return out_nodes.size(); }
  void run_output_nodes_if_ready(std::shared_ptr<frame::Context> context);

  void incr_input_num() { input_num++; }

  int get_input_num() { return input_num; }
  virtual const std::string type() { return "cpu"; }

 private:
  int input_num = 0;
  std::vector<Node*> out_nodes;
  std::string name;

 protected:
  std::shared_ptr<bvar::LatencyRecorder> recorder;
};

class Graph {
 private:
  Node* root_node = nullptr;
  std::vector<Node*> all_nodes;
  std::string name = "graph_default_name";

 public:
  static Graph& instance() {
    static Graph g;
    return g;
  }
  void set_name(const std::string& name) { this->name = name; }
  int init(const std::string& path) { return 0; }
  int init(Node* node, const std::string& name = "") {
    if (!name.empty()) {
      this->name = name;
    }
    root_node = node;
    all_nodes.clear();
    auto cur_node = root_node;
    std::queue<Node*> q;
    q.push(cur_node);
    std::set<Node*> set;
    while (!q.empty()) {
      cur_node = q.front();
      q.pop();
      if (set.find(cur_node) == set.end()) {
        set.insert(cur_node);
        all_nodes.push_back(cur_node);
      }
      for (auto node : cur_node->get_output_nodes()) {
        q.push(node);
      }
    }
    VLOG_APP(ERROR) << "graph: " << name << "all_nodes size: " << all_nodes.size();
    std::cout << "graph: " << name << " all_nodes size: " << all_nodes.size() << endl;
    return 0;
  }
  template <typename Request, typename Response>
  int run(brpc::Controller* cntl, const Request* request, Response* response,
          google::protobuf::Closure* done) {
    // 1. 构造图的上下文
    auto context = std::make_shared<frame::Context>(cntl, request, response, done);
    context->Init();
    if (root_node == nullptr) {
      VLOG_APP(ERROR) << "root_node is nullptr";
      return -1;
    }
    // auto node_timecost_monitor = context->monitor_ctx->mutable_node_timecost_monitor();
    for (auto node : all_nodes) {
      auto it = context->node_input_num_map.find(node);
      if (it == context->node_input_num_map.end()) {
        context->node_input_num_map[node] =
            std::make_shared<std::atomic<int>>(node->get_input_num());
        // VLOG_APP(ERROR) << "node: " << node->get_name() << " input_num:" <<
        // context->node_input_num_map[node]->load();
      }
      // node_timecost_monitor->insert({node->get_name(),
      // std::make_shared<dag::NodeTimeCostMonitor>()});
    }
    // 2. run
    root_node->run(context);
#ifdef Dag_Synchronize_Use

#ifdef DAG_THREAD_USE
    for (auto&& result : *context->mutable_future_res()) {
      auto res = result.get();  // wait
    }
#else
    for (auto& b : context->bt_vec()) {
      bthread_join(b, nullptr);
    }
#endif
#endif
    return 0;
  }
};
}  // namespace dag
