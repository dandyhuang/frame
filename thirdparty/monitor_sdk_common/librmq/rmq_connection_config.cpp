//
// Created by 黄伟锋 on 2019-04-22.
//

#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_connection_config.h"

#include "thirdparty/jsoncpp/json.h"

using namespace common;


bool RmqConnectionConfig::DeserializeConnectionConfig(const std::string &connection_config_json_str, RmqConnectionConfig *ret) {

  std::string data_str = connection_config_json_str;
  Json::CharReaderBuilder builder;
  scoped_ptr<Json::CharReader> reader(builder.newCharReader());
  Json::Value config_root;
  if (!reader->parse(data_str.c_str(), data_str.c_str() + data_str.size(), &config_root, NULL)) return false;

  RmqConnectionConfig config;
  config.producer_auth = config_root["producerAuth"].asBool();
  config.nextime = config_root["nexTime"].asInt();
  config.restart = config_root["restart"].asBool();
  config.topic_key = config_root["topicKey"].asString();
  config.producer = config_root["producer"].asBool();
  config.exchange = config_root["exchange"].asString();
  config.exchange_type = config_root["exchangeType"].asString();
  config.rate = config_root["rate"].asInt();
  config.queue = config_root["queue"].asString();
  config.routing_key = config_root["routingKey"].asString();
  config.thread_num = config_root["threadNum"].asInt();
  config.prefetch = config_root["prefetch"].asInt();
  config.priority = config_root["priority"].asBool();
  config.username = config_root["username"].asString();
  config.password = config_root["password"].asString();
  config.port = config_root["port"].asInt();

  std::vector<RmqVhostConfig> vhost_configs;
  Json::Value vhost_configs_root = config_root["configs"];
  for (unsigned int j = 0; j < vhost_configs_root.size(); ++j) {
    RmqVhostConfig vhost_config;

    vhost_config.vhost = vhost_configs_root[j]["vhost"].asString();
    std::vector<std::string> nodes;
    Json::Value vhost_nodes = vhost_configs_root[j]["nodes"];
    for (unsigned int k = 0; k < vhost_nodes.size(); ++ k) {
      nodes.push_back(vhost_nodes[k].asString());
    }
    vhost_config.nodes = nodes;

    vhost_configs.push_back(vhost_config);
  }
  config.configs = vhost_configs;

  *ret = config;

  return true;
}


bool RmqConnectionConfig::DeserializeConnectionConfigs(const std::string &connection_configs_json_str, std::vector<RmqConnectionConfig> *ret) {
  std::string data_str = connection_configs_json_str;
  Json::CharReaderBuilder builder;
  scoped_ptr<Json::CharReader> reader(builder.newCharReader());
  Json::Value root;
  if (!reader->parse(data_str.c_str(), data_str.c_str() + data_str.size(), &root, NULL)) return false;

  std::vector<RmqConnectionConfig> configs;

  for (unsigned int i = 0; i < root.size(); ++i) {
    Json::Value config_root = root[i];

    RmqConnectionConfig config;
    config.producer_auth = config_root["producerAuth"].asBool();
    config.nextime = config_root["nexTime"].asInt();
    config.restart = config_root["restart"].asBool();
    config.topic_key = config_root["topicKey"].asString();
    config.producer = config_root["producer"].asBool();
    config.exchange = config_root["exchange"].asString();
    config.exchange_type = config_root["exchangeType"].asString();
    config.rate = config_root["rate"].asInt();
    config.queue = config_root["queue"].asString();
    config.routing_key = config_root["routingKey"].asString();
    config.thread_num = config_root["threadNum"].asInt();
    config.prefetch = config_root["prefetch"].asInt();
    config.priority = config_root["priority"].asBool();
    config.username = config_root["username"].asString();
    config.password = config_root["password"].asString();
    config.port = config_root["port"].asInt();

    std::vector<RmqVhostConfig> vhost_configs;
    Json::Value vhost_configs_root = config_root["configs"];
    for (unsigned int j = 0; j < vhost_configs_root.size(); ++j) {
      RmqVhostConfig vhost_config;

      vhost_config.vhost = vhost_configs_root[j]["vhost"].asString();
      std::vector<std::string> nodes;
      Json::Value vhost_nodes = vhost_configs_root[j]["nodes"];
      for (unsigned int k = 0; k < vhost_nodes.size(); ++ k) {
        nodes.push_back(vhost_nodes[k].asString());
      }
      vhost_config.nodes = nodes;

      vhost_configs.push_back(vhost_config);
    }
    config.configs = vhost_configs;

    // append item to result
    configs.push_back(config);
  }

  *ret = configs;

  return true;
}
