// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "thirdparty/monitor_sdk_common/storage/sharding/fingerprint_sharding.h"

#include "thirdparty/monitor_sdk_common/hash/fingerprint.h"

namespace common {

FingerprintSharding::FingerprintSharding() {
}

FingerprintSharding::~FingerprintSharding() {
}

int FingerprintSharding::Shard(const std::string& key) {
    int shard_id = Fingerprint64(key) % (shard_num_);
    return shard_id;
}

COMMON_REGISTER_SHARDING_POLICY(FingerprintSharding);
}  // namespace util
