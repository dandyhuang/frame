#include "dag/graph.h"

#ifdef DAG_THREAD_USE
#include "common/thread_pool.h"
common::ThreadPool* g_pThreadPool = new (std::nothrow)common::ThreadPool(3);
#else
#include <brpc/channel.h>
#endif
extern std::vector<bthread_t> g_test_bt_vec;
extern std::vector<std::future<void*>> g_testfuture_res;
namespace dag {
static void* b_func(void* args_tmp) {
  Bargs* args = (Bargs*)args_tmp;
  // std::cout << " bthread run output node: " << args->node->get_name().c_str() << std::endl;
  args->node->run(args->context);
  delete args;
  return nullptr;
}
void Node::init(std::shared_ptr<::common::ConfigXml> config) {}
void Node::run(std::shared_ptr<frame::Context> context) {
  if (!skip(context)) {
    // VLOG_APP(ERROR) << "node:[" << name << "] do_service start; " << " type: " << type() << "
    // input_num:  " << input_num << " out_nodes size:" << out_nodes.size();
    int64_t start = butil::gettimeofday_us();
    do_service(context);
    recorder->operator<<(butil::gettimeofday_us() - start);
    // io nodes call run_output_nodes_if_ready themselves
    if (type() == std::string("cpu")) {
      run_output_nodes_if_ready(context);
    }
  } else {
    // VLOG_APP(ERROR) << "skip " << name;
    run_output_nodes_if_ready(context);
  }
}
int Node::do_service(std::shared_ptr<frame::Context> context) noexcept {
  // VLOG_APP(ERROR) << name << " do service; input_num:  " << input_num << " out_nodes size:" <<
  // out_nodes.size() ;
  return 0;
}
bool Node::skip(std::shared_ptr<frame::Context> context) { return false; }
bool Node::notify(std::shared_ptr<frame::Context> context) {
  auto it = context->node_input_num_map.find(this);
  if (it == context->node_input_num_map.end()) {
    VLOG_APP(ERROR) << "node:" << this
                    << " not found in node_input_num_map, this should not happen";
    return false;
  }
  auto input_num = it->second;
  int old_value = input_num->fetch_sub(1);
  // VLOG_APP(ERROR) << name << " notified; " << " old_value: " << old_value;
  if (old_value == 1) {
    return true;
  } else if (old_value <= 0) {
    VLOG_APP(ERROR) << "too many notify, old_value: " << old_value << ", this should not happen";
  }
  return false;
}
void Node::run_output_nodes_if_ready(std::shared_ptr<frame::Context> context) {
  int ready_nodes_num = 0;
  Node* last_ready_node = nullptr;
  for (size_t i = 0; i < out_nodes.size(); i++) {
    auto output_node = out_nodes[i];
    // VLOG_APP(ERROR) << name << " notify " << output_node->get_name();
    auto ready = output_node->notify(context);
    if (ready) {
      // VLOG_APP(ERROR) << output_node->get_name() << " is ready";
      ++ready_nodes_num;
      if (last_ready_node != nullptr) {
#ifdef DAG_THREAD_USE
        Bargs* args = new Bargs(last_ready_node, context);
        std::future<void*> res = std::move(g_pThreadPool->enqueue(b_func, args));
        // context->mutable_future_res()->emplace_back(res);
        // for (auto && result : results) {
        //     auto res = result.get();// wait
        // }
        g_testfuture_res.emplace_back(std::move(res));
#else
        bthread_t tid;
        Bargs* args = new Bargs(last_ready_node, context);
        bthread_start_background(&tid, &BTHREAD_ATTR_SMALL, b_func, args);
        g_test_bt_vec.push_back(tid);
        context->mutable_bt_vec()->push_back(tid);
#endif
      }
      last_ready_node = output_node;
    }
  }
  if (last_ready_node != nullptr) {
    last_ready_node->run(context);
  }
  return;
}
}  // namespace dag
