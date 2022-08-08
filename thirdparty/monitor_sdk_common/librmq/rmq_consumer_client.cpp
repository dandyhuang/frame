//
// Created by 黄伟锋 on 2019-04-27.
//

#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_client.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_consumer_client.h"

using namespace common;


RmqConsumerClient::RmqConsumerClient(struct ev_loop *loop) : RmqClient(loop)  {
  producer_ = false;
}


void RmqConsumerClient::OnRetryReady(const std::string &topic_key, const std::string &vhost) {
  if (producer_) return;

  RmqConnectionConfig config = topic_key_connection_config_map_[topic_key];
  std::shared_ptr<RmqVhostClient> inner_client = topic_key_vhost_client_map_[topic_key][vhost];
  ConsumeInnerClient(inner_client, config);
}


bool RmqConsumerClient::Consume(RmqConsumerHandler *handler) {
  handler_ = handler;

  for (auto &kv : topic_key_vhost_client_map_) {
    std::string topic_key = kv.first;

    std::map<std::string, std::shared_ptr<RmqVhostClient>> vhost_client_map = kv.second;
    RmqConnectionConfig config = topic_key_connection_config_map_[topic_key];
    for (auto &vhost_client_pair : vhost_client_map) {
      std::shared_ptr<RmqVhostClient> inner_client = vhost_client_pair.second;
      ConsumeInnerClient(inner_client, config);
    }
  }

  return true;
}


bool RmqConsumerClient::ConsumeInnerClient(std::shared_ptr<RmqVhostClient> inner_client, const RmqConnectionConfig &config) {

  auto success_cb = [this, inner_client](const std::string &consumertag) {
    std::string topic_key = exchange_topic_key_map_[inner_client->exchange()];
    std::string key = StringPrint("%s_%s", topic_key, inner_client->vhost());
    this->consume_ready_check_keys_set_.erase(key);

    if (IsConsumeReady() && !has_consuming_ready_ && !has_consuming_error_) {
      has_consuming_ready_ = true;
      has_consuming_error_ = false;
      handler_->OnReady();
    }
  };

  auto error_cb = [this, inner_client](const char *message) {
    std::string ret_message(message);

    if (!has_consuming_error_ && !has_consuming_ready_) {
      has_consuming_error_ = true;
      handler_->OnError(ret_message);
    }

  };

  auto message_cb = [this, inner_client](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {
    std::string exchange = message.exchange();
    std::string topic_key = exchange_topic_key_map_[exchange];

    if (!IsConsumeReady()) {
      inner_client->Reject(deliveryTag);
      return;
    }

    if (handler_->OnMessage(topic_key, std::string(message.body(), message.bodySize()))) {
      // acknowledge the message
      inner_client->Ack(deliveryTag);
      metric_.consumer_success(topic_key);
    } else {
      // reject the message
      inner_client->Reject(deliveryTag);
      metric_.consumer_fail(topic_key);
    }

  };

  inner_client->Consume(config.queue, success_cb,  error_cb, message_cb);

  std::string topic_key = exchange_topic_key_map_[inner_client->exchange()];
  consume_ready_check_keys_set_.insert(StringPrint("%s_%s", topic_key, inner_client->vhost()));

  return true;
}


bool RmqConsumerClient::IsConsumeReady() {
  return this->consume_ready_check_keys_set_.size() == 0;
}
