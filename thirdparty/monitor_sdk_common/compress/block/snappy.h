// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef COMMON_COMPRESS_BLOCK_SNAPPY_H
#define COMMON_COMPRESS_BLOCK_SNAPPY_H

#include <string>

#include "thirdparty/monitor_sdk_common/compress/block/block_compression.h"

namespace common {

class SnappyCompression : public BlockCompression {
    COMMON_DECLARE_UNCOPYABLE(SnappyCompression);

public:
    SnappyCompression();
    virtual ~SnappyCompression();

    virtual std::string GetName() {
        return "snappy";
    }

private:
    virtual bool DoCompress(const char* str, size_t length, std::string* out);
    virtual bool DoUncompress(const char* str, size_t length, std::string* out);
};
}  // namespace common
#endif  // COMMON_COMPRESS_BLOCK_SNAPPY_H
