#include <brpc/channel.h>
#include <google/protobuf/any.pb.h>

#include "dag/graph_context.h"
namespace frame {

class Context : public dag::GraphContext {
 public:
  Context(brpc::Controller* cntl, const google::protobuf::Message* input_request,
          google::protobuf::Message* input_response, google::protobuf::Closure* done) {}
#ifdef Dag_Synchronize_Use
        std::vector<bthread_t> bt_vec_;
        std::vector<std::thread> t_vec_;
#else

#endif
};
}  // namespace frame
