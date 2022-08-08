//
// Created by 黄伟锋 on 2019-04-22.
//

#include <algorithm>
#include <chrono>
#include <random>

#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_auth_request.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_client.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_connection_config.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_constants.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_remote_service.h"
#include "thirdparty/monitor_sdk_common/librmq/utils.h"
#include "thirdparty/monitor_sdk_common/system/threading/this_thread.h"

using namespace common;


RmqClient::RmqClient(struct ev_loop *loop) : AMQP::LibEvHandler(loop) {}


RmqClient::~RmqClient() {}


void RmqClient::SetAppname(const std::string &appname) {
    appname_ = appname;
    RmqHeaders::appname = appname;
}


void RmqClient::SetIdc(const std::string &idc) {
  idc_ = idc;
}


void RmqClient::SetServerHost(const std::string &server_host) {
    server_host_ = server_host;
}


void RmqClient::PutSecretKey(const std::string &topic_key, const std::string &secret_key) {
  topic_key_secret_key_map_[topic_key] = secret_key;
}


bool RmqClient::Initialize(std::function<void()> on_init_ready, std::function<void(const std::string &message)> on_init_error) {
  // 重置初始化相关的的成员变量
  Reset();

  on_init_ready_ = on_init_ready;
  on_init_error_ = on_init_error;

  // Init remote service base on topic_key_secret_key_map_
  remote_service_ = std::make_shared<RmqRemoteService>(topic_key_secret_key_map_);

  // Init inner clients
  InitializeInnerConfigs();
  InitializeRmqScheduledExecutor();
  InitializeInnerClients();
  InitializeMetric();

  // Init exchange_topic_key_map_
  for (auto &kv : topic_key_connection_config_map_) {
    std::string topic_key = kv.first;
    std::string exchange = kv.second.exchange;
    exchange_topic_key_map_[exchange] = topic_key;
  }

  return true;
}


void RmqClient::InitializeInnerConfigs() {
  for (auto &kv : topic_key_secret_key_map_) {
    std::string topic_key = kv.first;

    RmqAuthRequest auth_request;
    auth_request.appname = appname_;
    auth_request.topic_key = topic_key;
    auth_request.idc = idc_;
    auth_request.timestamp = StringPrint("%d", RmqTimeUtils::GetCurrentTimeMillis());
    auth_request.nonce = RmqConstants::NONCE;
    auth_request.version = RmqConstants::VERSION;
    auth_request.producer = producer_;

    RmqConnectionConfig config;
    if (!remote_service_->PostAuth(auth_request, server_host_, &config)) continue;

    topic_key_connection_config_map_[topic_key] = config;
    next_heartbeat_delay_ = config.nextime;
  }
}


void RmqClient::InitializeMetric() {
  metric_.appname = appname_;
  metric_.topic_key = (*topic_key_secret_key_map_.begin()).first;
  metric_.idc = idc_;
  metric_.version = RmqConstants::VERSION;
  metric_.nonce = RmqConstants::NONCE;

  for (auto &kv : topic_key_connection_config_map_) {
    std::string topic_key = kv.first;
    if (producer_) {
      metric_.add_producer(topic_key);
    } else {
      metric_.add_consumer(topic_key);
    }
  }

  scheduled_executor_->ScheduleWithDynamicDelay([this](bool *terminated) {
    RmqMetric tmp_metric;
    metric_.clone_and_reset(&tmp_metric);
    std::vector<RmqConnectionConfig> configs;
    if (remote_service_->PostHeartbeat(tmp_metric, server_host_, &configs)) {
      if (configs.size() > 0) {
        next_heartbeat_delay_ = configs.begin()->nextime;
      }
    }
  }, next_heartbeat_delay_, &next_heartbeat_delay_);
}


void RmqClient::InitializeRmqScheduledExecutor() {
  int pool_size = 5;
  for (auto &kv : topic_key_connection_config_map_) {
    RmqConnectionConfig config = kv.second;
    pool_size += config.configs.size();
  }
  scheduled_executor_ = std::make_shared<RmqScheduledExecutor>(pool_size);
}


void RmqClient::InitializeInnerClients() {
  for (auto &kv : topic_key_connection_config_map_) {
    std::string topic_key = kv.first;
    RmqConnectionConfig config = kv.second;

    std::map<std::string, std::shared_ptr<RmqVhostClient>> vhost_client_map;
    for (RmqVhostConfig &vhost_config : config.configs) {
      std::string vhost = vhost_config.vhost;
      std::string node = vhost_config.nodes[0];

      RmqLogUtils::LogInfo(StringPrint("vhost select node: %s", node));

      // 生成新的 client
      std::shared_ptr<RmqVhostClient> client = GenerateNewClient(
          topic_key, node, vhost, config.username, config.password,
          config.exchange, config.exchange_type, config.queue, config.routing_key);
      vhost_client_map[vhost] = client;
      connection_topic_vhost_key_map_[client->connection()] = std::pair<std::string, std::string>(topic_key, vhost);

      // 添加连接成功检查项
      std::string key = StringPrint("%s_%s", topic_key, vhost);
      connection_ready_check_keys_set_.insert(key);
      channel_ready_check_keys_set_.insert(key);
      for (std::string &node : vhost_config.nodes) {
        connection_retry_check_set_map_[topic_key][vhost].insert(node);
      }
    }
    topic_key_vhost_client_map_[topic_key] = vhost_client_map;
  }
}


std::shared_ptr<RmqVhostClient> RmqClient::GenerateNewClient(
    const std::string &topic_key,
    const std::string &node,
    const std::string &vhost,
    const std::string &username,
    const std::string &password,
    const std::string &exchange,
    const std::string &exchange_type,
    const std::string &queue,
    const std::string &routing_key) {

  std::shared_ptr<RmqVhostClient> client(new RmqVhostClient(scheduled_executor_));

  auto on_channel_ready = [this, topic_key, vhost](AMQP::TcpConnection *connection) {
    connection_topic_vhost_key_map_[connection] = std::pair<std::string, std::string>(topic_key, vhost);
    onChannelReady(connection);
  };
  auto on_channel_error = [this](AMQP::TcpConnection *connection, const std::string& message) {
    onChannelError(connection, message);
  };

  client->Initialize(this, node, vhost, username, password,
                     exchange, exchange_type, queue, routing_key,
                     on_channel_ready, on_channel_error);

  return client;
}


void RmqClient::Reset() {
  exchange_topic_key_map_.clear();
  topic_key_connection_config_map_.clear();
  topic_key_vhost_client_map_.clear();
  connection_topic_vhost_key_map_.clear();
  connection_ready_check_keys_set_.clear();
  channel_ready_check_keys_set_.clear();
  on_init_ready_ = nullptr;
  on_init_error_ = nullptr;
  init_fail_ = false;
}


bool RmqClient::IsInitReady() {
  return (connection_ready_check_keys_set_.size() == 0 && channel_ready_check_keys_set_.size() == 0);
}


void RmqClient::onError(AMQP::TcpConnection *connection, const char *message) {
  RmqLogUtils::LogInfo(StringPrint("onError: %s", message));
}


void RmqClient::onConnected(AMQP::TcpConnection *connection) {
  RmqLogUtils::LogInfo(StringPrint("onConnected: %s", connection_topic_vhost_key_map_[connection].first));
}


void RmqClient::onReady(AMQP::TcpConnection *connection) {
  RmqLogUtils::LogInfo(StringPrint("onReady: %s", connection_topic_vhost_key_map_[connection].first));

  if (init_fail_ || init_success_) return;

  std::string topic_key = connection_topic_vhost_key_map_[connection].first;
  std::string vhost = connection_topic_vhost_key_map_[connection].second;
  connection_ready_check_keys_set_.erase(StringPrint("%s_%s", topic_key, vhost));
}


void RmqClient::onClosed(AMQP::TcpConnection *connection) {
  RmqLogUtils::LogInfo(StringPrint("onClosed: %s", connection_topic_vhost_key_map_[connection].first));
}


void RmqClient::onDetached(AMQP::TcpConnection *connection) {
  RmqLogUtils::LogInfo(StringPrint("onDetached: %s", connection_topic_vhost_key_map_[connection].first));

  if (init_fail_) return;

  std::string topic_key = connection_topic_vhost_key_map_[connection].first;
  std::string vhost = connection_topic_vhost_key_map_[connection].second;
  std::shared_ptr<RmqVhostClient> client = topic_key_vhost_client_map_[topic_key][vhost];

  connection_topic_vhost_key_map_.erase(connection);

  if (!init_success_) {
    // 还没完成初始化

    // 处理候选 host
    connection_retry_check_set_map_[topic_key][vhost].erase(client->host());
    if (connection_retry_check_set_map_[topic_key][vhost].size() == 0) {
      init_fail_ = true;
      on_init_error_(StringPrint("fail to retry all hosts for topic_key(%s) and vhost(%s)", topic_key, vhost));
      return;
    }

    // 清除旧的连接 && 重建新连接
    std::string new_host = *connection_retry_check_set_map_[topic_key][vhost].begin();
    client->Retry(new_host);
    connection_topic_vhost_key_map_[client->connection()] = std::pair<std::string, std::string>(topic_key, vhost);

  } else {

    // 已经初始化过了
    ThisThread::Sleep(RmqConstants::CONNECTION_RETRY_INTERVAL);
    std::string old_host = client->host();
    for (RmqVhostConfig &config : topic_key_connection_config_map_[topic_key].configs) {
      if (config.vhost == vhost) {

        // 选择下一个 host
        auto old_itr = std::find(config.nodes.begin(), config.nodes.end(), old_host);
        int old_idx = std::distance(config.nodes.begin(), old_itr);
        int new_idx = (old_idx + 1) % config.nodes.size();
        std::string new_host = config.nodes[new_idx];

        // 重建新连接
        RmqLogUtils::LogInfo(StringPrint("Retry new host: %s while old host is: %s", new_host, old_host));
        client->Retry(new_host);
        connection_topic_vhost_key_map_[client->connection()] =
            std::pair<std::string, std::string>(topic_key, vhost);
      }
    }
  }
}


void RmqClient::onChannelReady(AMQP::TcpConnection *connection) {
  RmqLogUtils::LogInfo(StringPrint("onChannelReady: %s", connection_topic_vhost_key_map_[connection].first));

  if (init_fail_) return;

  std::string topic_key = connection_topic_vhost_key_map_[connection].first;
  std::string vhost = connection_topic_vhost_key_map_[connection].second;

  if (init_success_) {
    OnRetryReady(topic_key, vhost);
    return;
  }

  channel_ready_check_keys_set_.erase(StringPrint("%s_%s", topic_key, vhost));

  // 检查 client 是否初始化成功
  if (IsInitReady() && !init_success_) {
    init_fail_ = false;
    init_success_ = true;
    on_init_ready_();
  }
}


void RmqClient::onChannelError(AMQP::TcpConnection *connection, const std::string& message) {
  RmqLogUtils::LogInfo(StringPrint("onChannelError: %s", message));

  if (init_fail_ || init_success_) return;

  std::string topic_key = connection_topic_vhost_key_map_[connection].first;
  std::string vhost = connection_topic_vhost_key_map_[connection].second;
}
