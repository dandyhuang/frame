//
// Created by 黄伟锋 on 2019-04-10.
//

#ifndef COMMON_LIBCFG_CONFIG_LISTENER_H_
#define COMMON_LIBCFG_CONFIG_LISTENER_H_

#include "thirdparty/monitor_sdk_common/libcfg/property_item.h"
#include "thirdparty/monitor_sdk_common/libcfg/change_event_type.h"

namespace common {

// 配置变更监听器。
class ConfigListener {
 public:
  ConfigListener() {}
  virtual ~ConfigListener() {}

  virtual void EventReceived(const PropertyItem &item, const ChangeEventType &type) {}
};

} // namespace common

#endif // LIBCFG_CONFIG_LISTENER_H_



