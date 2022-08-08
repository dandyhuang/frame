//
// Created by 黄伟锋 on 2019-04-22.
//

#include "thirdparty/monitor_sdk_common/librmq/rmq_metric.h"
#include "thirdparty/monitor_sdk_common/librmq/utils.h"

#include "thirdparty/jsoncpp/json.h"

using namespace common;


void RmqMetric::add_producer(const std::string &topic_key) {
  producer_topics_.insert(topic_key);
}


void RmqMetric::add_consumer(const std::string &topic_key) {
  consumer_topics_.insert(topic_key);
}


// 设置重启状态
void RmqMetric::set_restart_state(const std::string &topic_key, const RmqRestartState &state) {
  restart_state_map_[topic_key] = state;
}


void RmqMetric::set_restart_state_map(const std::map<std::string, RmqRestartState> &restart_state_map) {
  restart_state_map_ = restart_state_map;
}


// 获取重启状态
RmqRestartState RmqMetric::get_restart_state(const std::string &topic_key) {
  if (restart_state_map_.find(topic_key) == restart_state_map_.end())
    return RmqRestartState::RUNNING;

  return restart_state_map_[topic_key];
}


// 发送成功一条数据，记录
void RmqMetric::producer_success(const std::string &topic_key) {
  if (pro_su_metrics.find(topic_key) == pro_su_metrics.end())
    pro_su_metrics[topic_key] = 0;

  pro_su_metrics[topic_key] ++;
}


// 发送数据异常记录
void RmqMetric::producer_fail(const std::string &topic_key) {
  if (pro_fa_metrics.find(topic_key) == pro_fa_metrics.end())
    pro_fa_metrics[topic_key] = 0;

  pro_fa_metrics[topic_key] ++;
}


// 消费数据成功，记录
void RmqMetric::consumer_success(const std::string &topic_key) {
  if (con_su_metrics.find(topic_key) == con_su_metrics.end())
    con_su_metrics[topic_key] = 0;

  con_su_metrics[topic_key] ++;
}


// 消费数据异常，记录
void RmqMetric::consumer_fail(const std::string &topic_key) {
  if (con_fa_metrics.find(topic_key) == con_fa_metrics.end())
    con_fa_metrics[topic_key] = 0;

  con_fa_metrics[topic_key] ++;
}


// 深度克隆，copy on write
bool RmqMetric::clone_and_reset(RmqMetric *ret) {

  //重置发送成功
  std::map<std::string, int> old_pro_su_metrics = pro_su_metrics;
  pro_su_metrics.clear();
  //重置发送失败
  std::map<std::string, int> old_pro_fa_metrics = pro_fa_metrics;
  pro_fa_metrics.clear();
  //重置消费成功
  std::map<std::string, int> old_con_su_metrics = con_su_metrics;
  con_su_metrics.clear();
  //重试消费失败
  std::map<std::string, int> old_con_fa_metrics = con_fa_metrics;
  con_fa_metrics.clear();

  for (std::string topic_key : producer_topics_) {
    if (old_pro_su_metrics.find(topic_key) == old_pro_su_metrics.end()) {
      old_pro_su_metrics[topic_key] = 0;
    }
    if (old_pro_fa_metrics.find(topic_key) == old_pro_fa_metrics.end()) {
      old_pro_fa_metrics[topic_key] = 0;
    }
  }

  for (std::string topic_key: consumer_topics_) {
    if (old_con_su_metrics.find(topic_key) == old_con_su_metrics.end()) {
      old_con_su_metrics[topic_key] = 0;
    }
    if (old_con_fa_metrics.find(topic_key) == old_con_fa_metrics.end()) {
      old_con_fa_metrics[topic_key] = 0;
    }
  }

  //构造上报参数
  RmqMetric rmq_metric;
  rmq_metric.pro_su_metrics = old_pro_su_metrics;
  rmq_metric.pro_fa_metrics = old_pro_fa_metrics;
  rmq_metric.con_su_metrics = old_con_su_metrics;
  rmq_metric.con_fa_metrics = old_con_fa_metrics;
  rmq_metric.set_restart_state_map(restart_state_map_);

  rmq_metric.appname = appname;
  rmq_metric.topic_key = topic_key;
  rmq_metric.idc = idc;
  rmq_metric.version = version;
  rmq_metric.nonce = nonce;

  //重置时间，为避免加锁，此处会有一些时间上的误差，但是不影响统计
  long end = RmqTimeUtils::GetCurrentTimeMillis();
  rmq_metric.start = start;
  rmq_metric.end = end;
  start = end;

  *ret = rmq_metric;

  return true;
}


std::string RmqMetric::ToJson() {
  Json::Value root;
  root["appname"] = appname;
  root["end"] = end;
  root["idc"] = idc;
  root["nonce"] = nonce;
  root["token"] = token;
  root["topicKey"] = topic_key;
  root["version"] = version;

  // prepare empty root
  Json::Value empty_root;
  Json::Reader reader;
  reader.parse("{}", empty_root);

  Json::Value pro_su_metrics_root;
  for (auto &kv : pro_su_metrics) {
    pro_su_metrics_root[kv.first] = kv.second;
  }
  root["proSuMetrics"] = pro_su_metrics_root.isNull() ? empty_root : pro_su_metrics_root;

  Json::Value pro_fa_metrics_root;
  for (auto &kv : pro_fa_metrics) {
    pro_fa_metrics_root[kv.first] = kv.second;
  }
  root["proFaMetrics"] = pro_fa_metrics_root.isNull() ? empty_root : pro_fa_metrics_root;

  Json::Value con_su_metrics_root;
  for (auto &kv : con_su_metrics) {
    con_su_metrics_root[kv.first] = kv.second;
  }
  root["conSuMetrics"] = con_su_metrics_root.isNull() ? empty_root : con_su_metrics_root;

  Json::Value con_fa_metrics_root;
  for (auto &kv : con_fa_metrics) {
    con_fa_metrics_root[kv.first] = kv.second;
  }
  root["conFaMetrics"] = con_fa_metrics_root.isNull() ? empty_root : con_fa_metrics_root;

  Json::Value restart_state_map_root;
  for (auto &kv : restart_state_map_) {
    restart_state_map_root[kv.first] = kv.second;
  }
  root["restartStateMap"] = restart_state_map_root.isNull() ? empty_root : restart_state_map_root;

  return root.toStyledString();
}
