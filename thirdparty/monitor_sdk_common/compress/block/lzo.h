// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef COMMON_COMPRESS_BLOCK_LZO_H
#define COMMON_COMPRESS_BLOCK_LZO_H

#include <stdint.h>

#include <string>

#include "thirdparty/monitor_sdk_common/compress/block/block_compression.h"

namespace common {

class LzoCompression : public BlockCompression {
    COMMON_DECLARE_UNCOPYABLE(LzoCompression);

public:
    LzoCompression();
    virtual ~LzoCompression();

    virtual std::string GetName() {
        return "lzo";
    }

private:
    virtual bool DoCompress(const char* str, size_t length, std::string* out);
    virtual bool DoUncompress(const char* str, size_t length, std::string* out);

    unsigned char* uncompressed_buff_;
    uint32_t un_buff_size_;

    unsigned char* compressed_buff_;
    uint32_t c_buff_size_;
    void* wrkmem_;
};
}  // namespace common
#endif  // COMMON_COMPRESS_BLOCK_LZO_H
