// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef COMMON_COMPRESS_BLOCK_BLOCK_COMPRESSION_H
#define COMMON_COMPRESS_BLOCK_BLOCK_COMPRESSION_H

#include <string>

#include "thirdparty/monitor_sdk_common/base/class_registry.h"
#include "thirdparty/monitor_sdk_common/base/string/string_piece.h"
#include "thirdparty/monitor_sdk_common/base/uncopyable.h"

namespace common {

class BlockCompression {
    COMMON_DECLARE_UNCOPYABLE(BlockCompression);

public:
    BlockCompression();
    virtual ~BlockCompression();

    bool Compress(const char* str, size_t length, std::string* out);
    bool Compress(StringPiece sp, std::string* out);
    bool Uncompress(const char* str, size_t length, std::string* out);
    bool Uncompress(StringPiece sp, std::string* out);
    virtual std::string GetName() = 0;
    void SetMaxUnCompressedSize(size_t s) {
        max_unCompressed_size_ = s;
    }

protected:
    size_t max_unCompressed_size_;

private:
    virtual bool DoCompress(const char* str, size_t length, std::string* out) = 0;
    virtual bool DoUncompress(const char* str, size_t length, std::string* out) = 0;
};

COMMON_CLASS_REGISTRY_DEFINE(block_compression_registry, BlockCompression);

#define COMMON_REGISTER_BLOCK_COMPRESSION(class_name, algorithm_name) \
    COMMON_CLASS_REGISTRY_REGISTER_CLASS( \
        common::block_compression_registry, \
        common::BlockCompression, \
        algorithm_name, \
        class_name)

#define COMMON_CREATE_BLOCK_COMPRESSION(name) \
    COMMON_CLASS_REGISTRY_CREATE_OBJECT(block_compression_registry, name)

}  // namespace common
#endif  // COMMON_COMPRESS_BLOCK_BLOCK_COMPRESSION_H
