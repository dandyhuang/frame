//
// Created by 黄伟锋 on 2019-04-10.
//

#include "thirdparty/monitor_sdk_common/libcfg/config_effect_info.h"
#include "thirdparty/monitor_sdk_common/libcfg/utils.h"

#include "thirdparty/jsoncpp/json.h"

using namespace common;


std::string ConfigEffectInfo::SerializeList(std::vector<ConfigEffectInfo> infos) {

  Json::Value root;
  int idx = 0;
  for (ConfigEffectInfo info : infos) {
    Json::Value info_value;
    info_value["hostName"] = info.host_name;
    info_value["appLoc"] = info.app_loc;
    info_value["appName"] = info.app_name;
    info_value["appEnv"] = info.app_env;
    info_value["configVersion"] = info.config_version;
    info_value["configLastModified"] = info.config_last_modified;
    info_value["requestTimestamp"] = info.request_timestamp;
    info_value["responseTimestamp"] = info.response_timestamp;
    info_value["effectTimestamp"] = info.effect_timestamp;

    root[idx] = info_value;
    idx ++;
  }

  return CfgJsonUtils::ParseString(root);
}