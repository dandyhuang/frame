//
// Created by 黄伟锋 on 2019-04-10.
//

#ifndef MASTER_COMMON_LIBCFG_PROPERTY_SOURCE_H_
#define MASTER_COMMON_LIBCFG_PROPERTY_SOURCE_H_

#include <string>

namespace common {

class PropertySource {
 public:
  std::string app_name;
  std::string app_env;
  std::string config_version;
  std::string app_loc;
  std::string config_host;
  std::string config_agent;
  std::string config_monitor;
};

} // namespace common

#endif //MASTER_COMMON_LIBCFG_PROPERTY_SOURCE_H_
