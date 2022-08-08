// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef UTIL_SHARDING_SHARDING_H_
#define UTIL_SHARDING_SHARDING_H_

#include "thirdparty/monitor_sdk_common/base/uncopyable.h"
#include "thirdparty/monitor_sdk_common/base/class_registry/class_registry.h"

namespace common {
class ShardingPolicy {
 public:
  ShardingPolicy();
  virtual ~ShardingPolicy();

  virtual void SetShardingNumber(int shard_num) {
    shard_num_ = shard_num;
  }

  virtual int Shard(const std::string& key) = 0;

 protected:
  int shard_num_;

 private:
  COMMON_DECLARE_UNCOPYABLE(ShardingPolicy);
};

COMMON_CLASS_REGISTRY_DEFINE(sharding_policy_registry, ShardingPolicy);

#define COMMON_REGISTER_SHARDING_POLICY(class_name) \
    COMMON_CLASS_REGISTRY_REGISTER_CLASS( \
        common::sharding_policy_registry, \
        common::ShardingPolicy, \
        #class_name, \
        class_name)
}  // namespace common

#define COMMON_CREATE_SHARDING_POLICY(name_as_string) \
    COMMON_CLASS_REGISTRY_CREATE_OBJECT(common::sharding_policy_registry, name_as_string)

#endif  // UTIL_SHARDING_SHARDING_H_

