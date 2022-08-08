// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include "thirdparty/monitor_sdk_common/base/benchmark.h"
#include "thirdparty/monitor_sdk_common/crypto/uuid/uuid.h"

#include "thirdparty/glog/logging.h"

static void CreateCanonicalUUIDString(int n) {
    for (int i = 0; i < n; i++) {
        std::string uuid = common::CreateCanonicalUUIDString();
        VLOG(1) << uuid;
    }
}

COMMON_BENCHMARK(CreateCanonicalUUIDString)->ThreadRange(1, NumCPUs());
