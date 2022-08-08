//
// Created by 黄伟锋 on 2019-04-15.
//

#ifndef COMMON_LIBCFG_MODEL_CONFIG_VERSION_TIMESTAMP_H_
#define COMMON_LIBCFG_MODEL_CONFIG_VERSION_TIMESTAMP_H_

#include <string>


namespace common {

class ConfigVersionTimestamp {
 public:
  std::string app_name;
  std::string config_version;
  long last_modified = -1;
};

} // namespace common

#endif //COMMON_LIBCFG_MODEL_CONFIG_VERSION_TIMESTAMP_H_
