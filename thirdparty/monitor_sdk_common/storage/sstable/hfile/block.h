// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef COMMON_STORAGE_SSTABLE_HFILE_BLOCK_H
#define COMMON_STORAGE_SSTABLE_HFILE_BLOCK_H

#include <string>

#include "thirdparty/monitor_sdk_common/base/uncopyable.h"

namespace common {
class File;

namespace hfile {
class Block {
    COMMON_DECLARE_UNCOPYABLE(Block);

public:
    Block() {
    }
    virtual ~Block() = 0;

    // Append the block into the file.
    bool WriteToFile(File *fb);

    virtual const std::string EncodeToString() const {
        return std::string();
    }
    virtual bool DecodeFromString(const std::string &str) = 0;
};

}  // namespace hfile
}  // namespace common

#endif  // COMMON_STORAGE_SSTABLE_HFILE_BLOCK_H
