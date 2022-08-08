//
// Created by 黄伟锋 on 2019-04-27.
//

#ifndef COMMON_LIBRMQ_RMQ_CONSUMER_CLIENT_H_
#define COMMON_LIBRMQ_RMQ_CONSUMER_CLIENT_H_

#include "thirdparty/monitor_sdk_common/librmq/rmq_client.h"

namespace common {

class RmqConsumerHandler {
 public:
  virtual ~RmqConsumerHandler() {}
  virtual void OnReady() {}
  virtual void OnError(const std::string &message) {}
  virtual bool OnMessage(const std::string &topic_key, const std::string &message) {
    return true;
  }
};


class RmqConsumerClient : public RmqClient {
 public:
  RmqConsumerClient(struct ev_loop *loop);

  bool Consume(RmqConsumerHandler *handler);

 protected:
  void OnRetryReady(const std::string &topic_key, const std::string &vhost);

 private:
  bool ConsumeInnerClient(
      std::shared_ptr<RmqVhostClient> inner_client,
      const RmqConnectionConfig &config);

  bool IsConsumeReady();

  std::set<std::string> consume_ready_check_keys_set_;
  RmqConsumerHandler *handler_ = nullptr;
  bool has_consuming_ready_ = false;
  bool has_consuming_error_ = false;
};

} // namespace common

#endif //COMMON_LIBRMQ_RMQ_CONSUMER_CLIENT_H_
