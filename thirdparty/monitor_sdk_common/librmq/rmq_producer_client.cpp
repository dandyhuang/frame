//
// Created by 黄伟锋 on 2019-04-27.
//

#include <stdlib.h>

#include "thirdparty/monitor_sdk_common/librmq/rmq_producer_client.h"
#include "thirdparty/monitor_sdk_common/librmq/utils.h"

using namespace common;


RmqProducerClient::RmqProducerClient(struct ev_loop *loop) : RmqClient(loop) {
  producer_ = true;
}


void RmqProducerClient::OnRetryReady(const std::string &topic_key, const std::string &vhost) {}


bool RmqProducerClient::RegisterHandler(RmqProducerHandler *handler) {
  handler_ = handler;

  auto on_publish_success = [this](const std::string &exchange, const std::string &message) {
    std::string topic_key = exchange_topic_key_map_[exchange];
    handler_->OnSuccess(topic_key, message);
    metric_.producer_success(topic_key);
  };

  auto on_publish_error = [this](const std::string &exchange, const std::string &message) {
    std::string topic_key = exchange_topic_key_map_[exchange];
    handler_->OnError(topic_key, message);
    metric_.producer_fail(topic_key);
  };

  for (auto &kv : topic_key_vhost_client_map_) {
    std::string topic_key = kv.first;
    std::map<std::string, std::shared_ptr<RmqVhostClient>> vhost_client_map = kv.second;

    for (auto &skv : vhost_client_map) {
      std::string vhost = skv.first;
      std::shared_ptr<RmqVhostClient> inner_client = skv.second;
      inner_client->RegisterPublishCallback(on_publish_success, on_publish_error);
    }
  }

  return true;
}


bool RmqProducerClient::Publish(const std::string &topic_key, const std::string &message) {
  std::map<std::string, std::shared_ptr<RmqVhostClient>> vhost_client_map = topic_key_vhost_client_map_[topic_key];

  int vhost_idx = rand() % vhost_client_map.size();
  std::string vhost = RmqMapUtils<std::shared_ptr<RmqVhostClient>>::GetStringKeys(vhost_client_map)[vhost_idx];
  std::shared_ptr<RmqVhostClient> inner_client = vhost_client_map[vhost];

  inner_client->Publish(message);

  return true;
}