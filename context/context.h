#include "dag/graph_context.h"

class Context : public dag::GraphContext {
  Context(brpc::Controller* cntl, const zeus::proto::ZeusRequest* input_request,
          zeus::proto::ZeusResponse* input_response, google::protobuf::Closure* done);
};