//
// Created by 黄伟锋 on 2019-04-25.
//

#ifndef COMMON_LIBRMQ_RMQ_VHOST_CLIENT_H_
#define COMMON_LIBRMQ_RMQ_VHOST_CLIENT_H_

#include <functional>
#include <memory>
#include <mutex>
#include <string>

#include "thirdparty/monitor_sdk_common/librmq/rmq_scheduled_executor.h"

#include "thirdparty/amqp/amqpcpp.h"
#include "thirdparty/amqp/amqpcpp/libev.h"


namespace common {

class RmqVhostClient {
 public:
  RmqVhostClient(std::shared_ptr<RmqScheduledExecutor> scheduled_executor);
  ~RmqVhostClient();

  void Initialize(
      AMQP::LibEvHandler *handler,
      const std::string &host,
      const std::string &vhost,
      const std::string &username,
      const std::string &password,
      const std::string &exchange,
      const std::string &exchange_type,
      const std::string &queue,
      const std::string &routing_key,
      std::function<void(AMQP::TcpConnection *connection)> on_channel_ready,
      std::function<void(AMQP::TcpConnection *connection, const std::string& message)> on_channel_error);

  void Retry(const std::string &new_host);

  void Publish(const std::string &message);

  void RegisterPublishCallback(std::function<void(const std::string &exchange, const std::string &message)> on_publish_success,
      std::function<void(const std::string &exchange, const std::string &message)> on_publish_error);

  void Consume(
      const std::string &queue,
      std::function<void(const std::string &consumertag)> success_cb,
      std::function<void(const char *message)> error_cb,
      std::function<void(const AMQP::Message &message, uint64_t deliveryTag, bool redelivered)> message_cb);

  bool Ack(int deliveryTag);

  bool Reject(int deliveryTag);

  AMQP::TcpConnection* connection();
  AMQP::TcpChannel* channel();
  std::string host();
  std::string exchange();
  std::string exchange_type();
  std::string queue();
  std::string routing_key();
  std::string vhost();
  std::string username();
  std::string password();

 protected:
  RmqVhostClient() {}

  int deliver_message_count_ = 0;
  std::map<AMQP::TcpChannel*, std::map<int, std::string>> deliver_tag_message_map_;
  std::mutex deliver_mutex_;

 private:

  void DetectConnectionUsable();
  void Initialize();

  AMQP::TcpConnection *connection_ = nullptr;
  AMQP::TcpChannel *channel_ = nullptr;

  std::string exchange_;
  std::string exchange_type_;
  std::string queue_;
  std::string routing_key_;
  std::string host_;
  std::string vhost_;
  std::string username_;
  std::string password_;
  AMQP::LibEvHandler *handler_;

  std::function<void(AMQP::TcpConnection *connection)> on_channel_ready_;
  std::function<void(AMQP::TcpConnection *connection, const std::string& message)> on_channel_error_;
  std::function<void(const std::string &exchange, const std::string &message)> on_publish_success_;
  std::function<void(const std::string &exchange, const std::string &message)> on_publish_error_;

  std::shared_ptr<RmqScheduledExecutor> scheduled_executor_;
};


} // namespace common

#endif //COMMON_LIBRMQ_RMQ_VHOST_CLIENT_H_
