//
// Created by 黄伟锋 on 2019-04-10.
//

#ifndef COMMON_LIBCFG_GET_CONFIG_REQUEST_H_
#define COMMON_LIBCFG_GET_CONFIG_REQUEST_H_

#include <string>

#include "thirdparty/monitor_sdk_common/libcfg/utils.h"

#include "thirdparty/jsoncpp/json.h"


namespace common {

class GetConfigRequest {
 public:
  std::string app_name;
  std::string app_env;
  std::string config_version;
  std::string client_default_config_version;
  long last_modified;
  std::string config_host;
  std::string host_name;
  std::string ip;
  std::string app_loc;

  std::string ToString() const {
    Json::Value root;
    root["app_name"] = app_name;
    root["app_env"] = app_env;
    root["config_version"] = config_version;
    root["client_default_config_version"] = client_default_config_version;
    root["last_modified"] = last_modified ? "true" : "false";
    root["config_host"] = config_host;
    root["host_name"] = host_name;
    root["ip"] = ip;
    root["app_loc"] = app_loc;
    return CfgJsonUtils::ParseString(root);
  }
};

} // namespace common

#endif // LIBCFG_GET_CONFIG_REQUEST_H_
