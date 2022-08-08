//
// Created by 黄伟锋 on 2019-04-22.
//

#ifndef COMMON_LIBRMQ_RMQ_CONNECTION_CONFIG_H_
#define COMMON_LIBRMQ_RMQ_CONNECTION_CONFIG_H_

#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/librmq/rmq_vhost_config.h"


namespace common {

class RmqConnectionConfig {
 public:
  static bool DeserializeConnectionConfig(const std::string &connection_config_json_str, RmqConnectionConfig *ret);
  static bool DeserializeConnectionConfigs(const std::string &connection_configs_json_str, std::vector<RmqConnectionConfig> *ret);

  // 是否生产白名单 
  bool producer_auth;

  //下一次上报时间
  long nextime;

  // 是否执行重启 
  bool restart;

  // topic key 
  std::string topic_key;

  // 生产还是发送 
  bool producer;

  // exchange名称 
  std::string exchange;

  // exchange类型 
  std::string exchange_type;

  // 允许发送速率，0表示不限速,这是一个总限速 
  int rate;

  // queue 
  std::string queue;

  // routing key 
  std::string routing_key;

  // thread num 
  int thread_num;

  // prefetch 
  int prefetch;

  // 是否启用优先级 
  bool priority;

  // 连接用户名 
  std::string username;

  // 连接密码
  std::string password;

  // 连接端口 
  int port;

  // 连接的vhost配置 
  std::vector<RmqVhostConfig> configs;
};

} // namespace common

#endif //COMMON_LIBRMQ_RMQ_CONNECTION_CONFIG_H_
