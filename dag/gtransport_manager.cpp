#include "dag/gtransport_manager.h"

#include <iostream>

#include "common/config_xml.h"
#include "common/loghelper.h"
#include "dag/global.h"

namespace dag {
using namespace vlog;
GtransportManager& GtransportManager::instance() {
  static GtransportManager inst;
  return inst;
}

void GtransportManager::init(const std::string& conf_path) noexcept {
  dag::ConfigXml xml_conf;
  xml_conf.init(conf_path);
  dag::ConfigXml channel_config;
  if (!xml_conf.Find("channel_config", channel_config)) {
    VLOG_APP(ERROR) << "channel_config not found in " << conf_path;
    return false;
  }
  dag::ConfigXml channel_conf;
  if (!channel_config.Child("channel", channel_conf)) {
    VLOG_APP(ERROR) << "channel not exist in channel_config in file: " << conf_path;
    return false;
  }
  bool has_next = false;
  do {
    std::string channel_name = "";
    channel_conf.Attr<std::string>("channel_name", channel_name);
    std::cout << "channel_name: " << channel_name << std::endl;
    global::transport_name_vec.push_back(channel_name);
    std::string bns = "";
    channel_conf.Attr<std::string>("bns", bns);
    std::string lb = "";
    channel_conf.Attr<std::string>("load_balancer", lb);
    int timeout = 0;
    channel_conf.Attr<int>("timeout_ms", timeout);
    int backup_timeout = 0;
    channel_conf.Attr<int>("backup_timeout_ms", backup_timeout);
    std::string connection_type = "";
    channel_conf.Attr<std::string>("connection_type", connection_type);
    int max_retry = 0;
    channel_conf.Attr<int>("max_retry", max_retry);
    std::string protocol = "";
    channel_conf.Attr<std::string>("protocal", protocol);
    std::string uri = "";
    channel_conf.Attr<std::string>("uri", uri);
    if (!bns.empty()) {
      brpc::ChannelOptions options;
      options.protocol = protocol.c_str();
      options.connection_type = connection_type.c_str();
      options.timeout_ms = timeout;
      options.max_retry = max_retry;
      options.backup_request_ms = backup_timeout;
      std::shared_ptr<brpc::Channel> channel(new brpc::Channel());
      if (channel->Init(bns.c_str(), lb.c_str(), &options) != 0) {
        VLOG_APP(ERROR) << "Fail to initialize " << channel_name << " channel";
        return;
      }
      if (channel_map.find(channel_name) == channel_map.end()) {
        VLOG_APP(ERROR) << "insert channel: " << channel_name << " bns: " << bns;
        std::cout << "insert channel: " << channel_name << " bns: " << bns << std::endl;
        channel_map[channel_name] = channel;
      } else {
        VLOG_APP(ERROR) << "channel " << channel_name << " already exists! error!!";
      }
    } else {
      brpc::ChannelOptions options;
      options.timeout_ms = 20;
      options.protocol = brpc::PROTOCOL_HTTP;  // or brpc::PROTOCOL_H2
      auto channel = std::make_shared<brpc::Channel>();
      if (channel->Init(uri.c_str(), &options) != 0) {
        VLOG_APP(ERROR) << "Fail to initialize http channel, _transport_name" << channel_name;
      }

      if (channel_map.find(channel_name) == channel_map.end()) {
        VLOG_APP(ERROR) << "insert channel: " << channel_name << " uri: " << uri;
        std::cout << "insert channel: " << channel_name << " uri: " << uri << std::endl;
        channel_map[channel_name] = channel;
      } else {
        VLOG_APP(ERROR) << "channel: " << channel_name << " already exists! error!!";
      }
    }
    has_next = channel_conf.Next("channel", channel_conf);
  } while (has_next);
  VLOG_APP(ERROR) << "channel_map size: " << channel_map.size();
  std::cout << "channel_map size: " << channel_map.size() << std::endl;
}

std::shared_ptr<brpc::Channel> GtransportManager::get_transport(const std::string& transport_name) {
  auto it = channel_map.find(transport_name);

  if (it != channel_map.end()) {
    return it->second;
  } else {
    VLOG_APP(ERROR) << "transport_name: " << transport_name.c_str() << " not found";
    return std::shared_ptr<brpc::Channel>();
  }
}
}  // end of namespace dag
