// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "thirdparty/monitor_sdk_common/storage/sstable/hfile/block.h"

#include "thirdparty/monitor_sdk_common/storage/file/file.h"

#include "thirdparty/glog/logging.h"

namespace common {
namespace hfile {

Block::~Block() {}

bool Block::WriteToFile(File *fb) {
    const std::string &str = EncodeToString();
    if (str.empty()) {
        return true;
    }
    int64_t s = fb->Write(str.c_str(), str.length());
    if (s != static_cast<int64_t>(str.length()))
        LOG(WARNING)<< "fail to write buffer";
    return true;
}

}  // namespace hfile
}  // namespace common
