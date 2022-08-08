//
// Created by 黄伟锋 on 2019-04-15.
//

#ifndef COMMON_LIBCFG_MODEL_GET_CONFIG_RESULT_H_
#define COMMON_LIBCFG_MODEL_GET_CONFIG_RESULT_H_

#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/libcfg/config_version_timestamp.h"
#include "thirdparty/monitor_sdk_common/libcfg/property_item.h"


namespace common {

class GetConfigResult {
 public:
  long last_modified;
  long request_timestamp;
  long response_timestamp;
  std::vector<PropertyItem> configs;
  std::string real_config_version;

};

} // namespace common

#endif //COMMON_LIBCFG_MODEL_GET_CONFIG_RESULT_H_

