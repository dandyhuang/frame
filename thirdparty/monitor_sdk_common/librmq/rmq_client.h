//
// Created by 黄伟锋 on 2019-04-22.
//

#ifndef COMMON_LIBRMQ_RMQ_CLIENT_H_
#define COMMON_LIBRMQ_RMQ_CLIENT_H_

#include <map>
#include <memory>
#include <set>
#include <string>

#include "thirdparty/monitor_sdk_common/librmq/rmq_metric.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_remote_service.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_vhost_client.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_scheduled_executor.h"


namespace common {

class RmqClient : public AMQP::LibEvHandler {

 public:
  RmqClient(struct ev_loop *loop);
  virtual ~RmqClient();

  void SetAppname(const std::string &appname);
  void SetIdc(const std::string &idc);
  void SetServerHost(const std::string &server_host);

  void PutSecretKey(const std::string &topic_key, const std::string &secret_key);

  // 同步初始化，超时返回 false，timeout 单位是毫秒
  bool Initialize(std::function<void()> on_init_ready, std::function<void(const std::string &message)> on_init_error);

 protected:

  virtual void OnRetryReady(const std::string &topic_key, const std::string &vhost) {}

  std::shared_ptr<RmqRemoteService> remote_service_;

  std::string appname_;
  std::string idc_;
  std::string server_host_;
  bool producer_;
  int next_heartbeat_delay_ = 10 * 1000;
  RmqMetric metric_;

  std::map<std::string, std::string> topic_key_secret_key_map_;

  // 与初始化相关的的成员变量
  std::map<std::string, std::string> exchange_topic_key_map_;
  std::map<std::string, RmqConnectionConfig> topic_key_connection_config_map_;
  std::map<std::string, std::map<std::string, std::shared_ptr<RmqVhostClient>>> topic_key_vhost_client_map_;
  // 根据 connection 反查 topic 和 vhost
  std::map<AMQP::TcpConnection*, std::pair<std::string, std::string>> connection_topic_vhost_key_map_;

  // 初始化检查项
  std::set<std::string> connection_ready_check_keys_set_;
  std::map<std::string, std::map<std::string, std::set<std::string>>> connection_retry_check_set_map_;
  std::set<std::string> channel_ready_check_keys_set_;

 private:

  std::function<void()> on_init_ready_;
  std::function<void(const std::string &message)> on_init_error_;
  bool init_fail_ = false;
  bool init_success_ = false;

  // 所有 vhost client 共用一个 executor
  std::shared_ptr<RmqScheduledExecutor> scheduled_executor_;

  // 重置初始化相关的的成员变量，每次初始化都要重置
  void Reset();

  // 初始化多个 vhost client
  void InitializeInnerClients();

  void InitializeInnerConfigs();
  void InitializeRmqScheduledExecutor();
  void InitializeMetric();

  // 判断是否已经初始化成功过
  bool IsInitReady();

  std::shared_ptr<RmqVhostClient> GenerateNewClient(
      const std::string &topic_key,
      const std::string &node,
      const std::string &vhost,
      const std::string &username,
      const std::string &password,
      const std::string &exchange,
      const std::string &exchange_type,
      const std::string &queue,
      const std::string &routing_key);

  // tcp 连接出错
  void onError(AMQP::TcpConnection *connection, const char *message) override;
  // tcp 连接成功
  void onConnected(AMQP::TcpConnection *connection) override;
  // tcp 连接准备就绪
  void onReady(AMQP::TcpConnection *connection) override;
  // tcp 连接关闭
  void onClosed(AMQP::TcpConnection *connection) override;
  // tcp 连接分离
  void onDetached(AMQP::TcpConnection *connection) override;
  // tcp 连接对应的 amqp channel 创建成功
  void onChannelReady(AMQP::TcpConnection *connection);
  // tcp 连接对应的 amqp channel 创建失败
  void onChannelError(AMQP::TcpConnection *connection, const std::string& message);
};

} // namespace common

#endif //COMMON_LIBRMQ_RMQ_CLIENT_H_
