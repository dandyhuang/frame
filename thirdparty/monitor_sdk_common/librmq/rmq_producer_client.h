//
// Created by 黄伟锋 on 2019-04-27.
//

#ifndef COMMON_LIBRMQ_RMQ_PRODUCER_CLIENT_H_
#define COMMON_LIBRMQ_RMQ_PRODUCER_CLIENT_H_

#include "thirdparty/monitor_sdk_common/librmq/rmq_client.h"

namespace common {

class RmqProducerHandler {
 public:
  virtual ~RmqProducerHandler() {}
  virtual void OnError(const std::string &topic_key, const std::string &error_message) {}
  virtual void OnSuccess(const std::string &topic_key, const std::string &message) {}
};


class RmqProducerClient : public RmqClient {
 public:
  RmqProducerClient(struct ev_loop *loop);

  bool RegisterHandler(RmqProducerHandler *handler);
  bool Publish(const std::string &topic_key, const std::string &message);

 protected:
  void OnRetryReady(const std::string &topic_key, const std::string &vhost);

 private:
  RmqProducerHandler *handler_ = nullptr;
};

} // namespace common

#endif //COMMON_LIBRMQ_RMQ_PRODUCER_CLIENT_H_
