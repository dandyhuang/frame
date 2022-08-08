//
// Created by 黄伟锋 on 2019-04-10.
//

#ifndef COMMON_LIBCFG_CHANGE_EVENT_TYPE_H_
#define COMMON_LIBCFG_CHANGE_EVENT_TYPE_H_

namespace common {

// 配置变更事件类型。
enum ChangeEventType {
  // A item was added.
  ITEM_ADDED,

  // A item's data was changed
  ITEM_UPDATED,

  // A item was removed
  ITEM_REMOVED
};

} // namespace common

#endif // COMMON_LIBCFG_CHANGE_EVENT_TYPE_H_
