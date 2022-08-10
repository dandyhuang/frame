#include <brpc/channel.h>
#include <google/protobuf/any.pb.h>

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

#include "dag/graph_context.h"
namespace frame {

class Context : public dag::GraphContext {
 public:
  Context(brpc::Controller* cntl, const google::protobuf::Message* input_request,
          google::protobuf::Message* input_response, google::protobuf::Closure* done) {}
  std::vector<bthread_t>* mutable_bt_vec() { return &bt_vec_; }
  const std::vector<bthread_t>& bt_vec() { return bt_vec_; }
  std::vector<std::thread>* mutable_t_vec() { return &t_vec_; }
  std::vector<std::shared_ptr<std::future<void*>>>* mutable_future_res() { return &future_res_; }

 private:
  std::vector<bthread_t> bt_vec_;
  std::vector<std::thread> t_vec_;
  std::vector<std::shared_ptr<std::future<void*>>> future_res_;
};
}  // namespace frame
