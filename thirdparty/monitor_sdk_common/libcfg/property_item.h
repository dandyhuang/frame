//
// Created by 黄伟锋 on 2019-04-10.
//

#ifndef COMMON_LIBCFG_PROPERTY_ITEM_H_
#define COMMON_LIBCFG_PROPERTY_ITEM_H_

#include <sstream>
#include <string>

#include "thirdparty/monitor_sdk_common/libcfg/utils.h"

#include "thirdparty/jsoncpp/json.h"


namespace common {

// 配置项。
class PropertyItem {
 public:
  PropertyItem(const std::string &name, const std::string &value) {
    this->name = name;
    this->value = value;
  }

  std::string name;
  std::string value;
  std::string comment;

  std::string ToString() const {
    Json::Value root;
    root["name"] = name;
    root["value"] = value;
    root["comment"] = comment;

    return CfgJsonUtils::ParseString(root);
  }
};

} // namespace common

#endif // LIBCFG_PROPERTY_ITEM_H_
