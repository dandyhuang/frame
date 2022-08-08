// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "thirdparty/monitor_sdk_common/compress/block/snappy.h"

#include "thirdparty/snappy/snappy.h"

namespace common {
SnappyCompression::SnappyCompression() {}

SnappyCompression::~SnappyCompression() {}

bool SnappyCompression::DoCompress(const char* str, size_t length, std::string* out) {
    snappy::Compress(str, length, out);
    return true;
}

bool SnappyCompression::DoUncompress(const char* str, size_t length, std::string* out) {
    return snappy::Uncompress(str, length, out);
}

COMMON_REGISTER_BLOCK_COMPRESSION(SnappyCompression, "snappy");
}  // namespace common
