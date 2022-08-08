//
// Created by 黄伟锋 on 2019-04-10.
//

#ifndef COMMON_LIBCFG_CONFIG_EFFECT_INFO_H_
#define COMMON_LIBCFG_CONFIG_EFFECT_INFO_H_

#include <string>
#include <vector>


namespace common {

class ConfigEffectInfo {
 public:
  static std::string SerializeList(std::vector<ConfigEffectInfo> infos);

  std::string host_name;
  std::string app_loc;
  std::string app_name;
  std::string app_env;
  std::string config_version;
  long config_last_modified;
  long request_timestamp;
  long response_timestamp;
  long effect_timestamp;
};

} // namespace common

#endif // LIBCFG_CONFIG_EFFECT_INFO_H_
