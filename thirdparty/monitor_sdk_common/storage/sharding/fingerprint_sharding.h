// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef COMMON_STORAGE_SHARDING_FINGER_SHARDING_H
#define COMMON_STORAGE_SHARDING_FINGER_SHARDING_H

#include <string>

#include "thirdparty/monitor_sdk_common/storage/sharding/sharding.h"

namespace common {
class FingerprintSharding : public ShardingPolicy {
    COMMON_DECLARE_UNCOPYABLE(FingerprintSharding);

public:
    FingerprintSharding();
    virtual ~FingerprintSharding();

    virtual int Shard(const std::string& key);
};
}  // namespace util
#endif  // COMMON_STORAGE_SHARDING_FINGER_SHARDING_H
