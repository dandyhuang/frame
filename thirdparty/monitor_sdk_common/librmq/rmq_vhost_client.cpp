//
// Created by 黄伟锋 on 2019-04-25.
//

#include <openssl/opensslv.h>
#include <openssl/ssl.h>

#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/base/string/format.h"
#include "thirdparty/monitor_sdk_common/crypto/uuid/uuid.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_constants.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_vhost_client.h"
#include "thirdparty/monitor_sdk_common/librmq/utils.h"

using namespace common;


RmqVhostClient::RmqVhostClient(std::shared_ptr<RmqScheduledExecutor> scheduled_executor) {
  scheduled_executor_ = scheduled_executor;
}

RmqVhostClient::~RmqVhostClient() {
  if (connection_ != nullptr) {
    connection_->close();
//    delete connection_;
  }

  if (channel_ != nullptr) {
    channel_->invalidate_monitors();
    if (channel_->usable()) channel_->close();
    delete channel_;
  }

  on_channel_ready_ = nullptr;
  on_channel_error_ = nullptr;
  on_publish_success_ = nullptr;
  on_publish_error_ = nullptr;
}


void RmqVhostClient::Publish(const std::string &message) {
  std::unique_lock<std::mutex> lock(deliver_mutex_);

  if (!channel_) return;

  AMQP::Table table;
  table.set("messageId", common::CreateCanonicalUUIDString());
  table.set("timestamp", IntegerToString(RmqTimeUtils::GetCurrentTimeMillis()));
  table.set("_app.name", RmqHeaders::appname);
  table.set("_app.node", RmqHeaders::appnode);
  AMQP::Envelope envelope(message.data(), message.size());
  envelope.setHeaders(table);

  envelope.setPersistent(true);
  channel_->publish(exchange_, routing_key_, envelope);
  deliver_tag_message_map_[channel_][++ deliver_message_count_] = message;
}


void RmqVhostClient::Consume(
    const std::string &queue,
    std::function<void(const std::string &consumertag)> success_cb,
    std::function<void(const char *message)> error_cb,
    std::function<void(const AMQP::Message &message, uint64_t deliveryTag, bool redelivered)> message_cb) {

  // QOS 客户端预取长度
  channel_->setQos(RmqConstants::QOS_PREFETCH_COUNT);

  channel_->consume(queue)
  .onReceived(message_cb)
  .onSuccess(success_cb)
  .onError(error_cb);
}


bool RmqVhostClient::Ack(int deliveryTag) {
  return channel_->ack(deliveryTag);
}


bool RmqVhostClient::Reject(int deliveryTag) {
  // 拒绝后，重复分发
  return channel_->reject(deliveryTag, AMQP::requeue);
}


void RmqVhostClient::Initialize(
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
    std::function<void(AMQP::TcpConnection *connection, const std::string& message)> on_channel_error) {

  handler_ = handler;
  exchange_ = exchange;
  exchange_type_ = exchange_type;
  routing_key_ = routing_key;
  queue_ = queue;
  host_ = host;
  vhost_ = vhost;
  username_ = username;
  password_ = password;

  on_channel_ready_ = on_channel_ready;
  on_channel_error_ = on_channel_error;

  Initialize();
}


void RmqVhostClient::RegisterPublishCallback(std::function<void(const std::string &exchange, const std::string &message)> on_publish_success,
    std::function<void(const std::string &exchange, const std::string &message)> on_publish_error) {
  on_publish_success_ = on_publish_success;
  on_publish_error_ = on_publish_error;
}


void RmqVhostClient::Initialize() {

  // init the SSL library
#if OPENSSL_VERSION_NUMBER < 0x10100000L
  SSL_library_init();
#else
  OPENSSL_init_ssl(0, NULL);
#endif

  std::string addr_url = StringPrint("amqp://%s:%s@%s/%s", username_, password_, host_, vhost_);
  AMQP::Address address(addr_url);

  if (connection_ != nullptr) {
    connection_->close();
//    delete connection_;
  }
  connection_ = new AMQP::TcpConnection(handler_, address);

  AMQP::TcpChannel *target_channel = new AMQP::TcpChannel(connection_);

  target_channel->onReady([this, target_channel]() {
    target_channel->confirmSelect().onSuccess([this, target_channel]() {
      RmqLogUtils::LogInfo("confirmSelect onSuccess");

      deliver_mutex_.lock();

      // Clear last channel
      AMQP::TcpChannel *last_channel = channel_;

      if (last_channel != nullptr) {
        last_channel->invalidate_monitors();
        if (last_channel->usable()) last_channel->close();
        delete last_channel;
      }
      channel_ = target_channel;

      // channel declare
      AMQP::ExchangeType ex_type = AMQP::fanout;
      if (exchange_type_ == "topic") {
        ex_type = AMQP::topic;
      }
      channel_->declareExchange(exchange_, ex_type, AMQP::durable);
      channel_->declareQueue(queue_, AMQP::durable);
      channel_->bindQueue(exchange_, queue_, routing_key_);

      // 获取上个 channel 残余的消息
      std::map<int, std::string> remaining_deliver_tag_message_map;
      if (last_channel != nullptr) {
        if (deliver_tag_message_map_.size() > 0) {
          remaining_deliver_tag_message_map = deliver_tag_message_map_[last_channel];
          deliver_tag_message_map_.erase(last_channel);
        }
      }

      deliver_message_count_ = 0;

      deliver_mutex_.unlock();

      // ready 回调
      if (on_channel_ready_) {
        on_channel_ready_(connection_);
      }

      // 清空上次连接残留的 message，统一当作 nack 处理
      if (remaining_deliver_tag_message_map.size() > 0) {
        for (auto &kv : remaining_deliver_tag_message_map) {
          RmqLogUtils::LogInfo(StringPrint("remaining size: %d", remaining_deliver_tag_message_map.size()));
          on_publish_error_(exchange_, kv.second);
        }
      }

    }).onAck([this, target_channel](uint64_t deliveryTag, bool multiple) {

      std::unique_lock<std::mutex> lock(deliver_mutex_);
      if (deliver_tag_message_map_.find(target_channel) == deliver_tag_message_map_.end()) return;

      int start = deliver_tag_message_map_[target_channel].begin()->first;
      int deliver_tag = int(deliveryTag);
      for (int i = start; i <= deliver_tag; i++) {
        if (on_publish_success_) {
          if (deliver_tag_message_map_[target_channel].find(i) != deliver_tag_message_map_[target_channel].end()) {
            on_publish_success_(exchange_, deliver_tag_message_map_[target_channel][i]);
          }
        }
        deliver_tag_message_map_[target_channel].erase(i);
      }

    }).onNack([this, target_channel](uint64_t deliveryTag, bool multiple, bool requeue) {

      std::unique_lock<std::mutex> lock(deliver_mutex_);
      if (deliver_tag_message_map_.find(target_channel) == deliver_tag_message_map_.end()) return;

      int start = deliver_tag_message_map_[target_channel].begin()->first;
      int deliver_tag = int(deliveryTag);
      for (int i = start; i <= deliver_tag; i++) {
        if (on_publish_error_) {
          if (deliver_tag_message_map_[target_channel].find(i) != deliver_tag_message_map_[target_channel].end()) {
            on_publish_error_(exchange_, deliver_tag_message_map_[target_channel][i]);
          }
        }
        deliver_tag_message_map_[target_channel].erase(i);
      }

    });

  });

  target_channel->onError([this, target_channel](const char *message) {
    // 错误回调
    if (on_channel_error_) {
      on_channel_error_(connection_, std::string(message));
    }
  });

  // 定时检测连接情况
  DetectConnectionUsable();
}


void RmqVhostClient::Retry(const std::string &new_host) {
  host_ = new_host;

  Initialize();
}


void RmqVhostClient::DetectConnectionUsable() {
  scheduled_executor_->ScheduleWithFixedDelay([this](bool *terminated) {
    bool usable = connection_->usable();
    RmqLogUtils::LogInfo(StringPrint("usable: %s ,size: %d, host: %s",
        usable ? "connection usable" : "connection not usable", deliver_tag_message_map_.size(), host_));
  }, RmqConstants::CHECK_USABLE_INTERVAL, RmqConstants::CHECK_USABLE_INTERVAL);
}


AMQP::TcpConnection* RmqVhostClient::connection() {
  return connection_;
}


AMQP::TcpChannel* RmqVhostClient::channel() {
  return channel_;
}


std::string RmqVhostClient::host() {
  return host_;
}


std::string RmqVhostClient::exchange() {
  return exchange_;
}


std::string RmqVhostClient::exchange_type() {
  return exchange_type_;
}


std::string RmqVhostClient::queue() {
  return queue_;
}


std::string RmqVhostClient::routing_key() {
  return routing_key_;
}


std::string RmqVhostClient::vhost() {
  return vhost_;
}

std::string RmqVhostClient::username() {
  return username_;
}


std::string RmqVhostClient::password() {
  return password_;
}