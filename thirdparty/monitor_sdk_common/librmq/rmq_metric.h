//
// Created by 黄伟锋 on 2019-04-22.
//

#ifndef COMMON_LIBRMQ_RMQ_METRIC_H_
#define COMMON_LIBRMQ_RMQ_METRIC_H_

#include <atomic>
#include <map>
#include <set>
#include <string>

#include "thirdparty/monitor_sdk_common/librmq/rmq_restart_state.h"


namespace common {

class RmqMetric {
 public:

  void add_producer(const std::string &topic_key);
  void add_consumer(const std::string &topic_key);

  // 设置重启状态
  void set_restart_state(const std::string &topic_key, const RmqRestartState &state);
  void set_restart_state_map(const std::map<std::string, RmqRestartState> &restart_state_map);

  // 获取重启状态
  RmqRestartState get_restart_state(const std::string &topic_key);

  // 发送成功一条数据，记录
  void producer_success(const std::string &topic_key);

  // 发送数据异常记录
  void producer_fail(const std::string &topic_key);

  // 消费数据成功，记录
  void consumer_success(const std::string &topic_key);

  // 消费数据异常，记录
  void consumer_fail(const std::string &topic_key);

  // 深度克隆，copy on write
  bool clone_and_reset(RmqMetric *ret);

  // 序列化成 json 字符串
  std::string ToJson();

  //应用名
  std::string appname;

  //topic key
  std::string topic_key;

  //客户端idc
  std::string idc;

  //客户端版本
  std::string version;

  //本次记录开始时间
  long start;

  //本次记录结束时间
  long end;

  //token
  std::string token;
  std::string nonce;

  // 发送成功结果
  std::map<std::string, int> pro_su_metrics;

  // 发送异常记录
  std::map<std::string, int> pro_fa_metrics;

  // 消费成功记录
  std::map<std::string, int> con_su_metrics;

  // 消费异常记录
  std::map<std::string, int> con_fa_metrics;


 private:
  //重启状态记录
  std::map<std::string, RmqRestartState> restart_state_map_;

  std::set<std::string> consumer_topics_;
  std::set<std::string> producer_topics_;
};

} // namespace common

#endif //COMMON_LIBRMQ_RMQ_METRIC_H_
