// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef COMMON_STORAGE_SSTABLE_WRITER_UNSORTED_SSTABLE_WRITER_H
#define COMMON_STORAGE_SSTABLE_WRITER_UNSORTED_SSTABLE_WRITER_H

#include <map>
#include <string>

#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/storage/sstable/types.h"
#include "thirdparty/monitor_sdk_common/storage/sstable/writer/base_sstable_writer.h"

namespace common {
class DataBlock;
class DataIndex;
class File;

class UnsortedSSTableWriter : public SSTableWriter {
    COMMON_DECLARE_UNCOPYABLE(UnsortedSSTableWriter);

public:
    explicit UnsortedSSTableWriter(const SSTableWriteOption &option);
    ~UnsortedSSTableWriter();

    virtual bool Add(const std::string &key, const std::string &value);
    virtual void AddMetaData(const std::string &key, const std::string &value);
    virtual bool Flush();

private:
    bool WriteBlockAndUpdateIndex();

    common::scoped_ptr<File> file_base_;
    bool failed_;
    common::scoped_ptr<hfile::DataBlock> block_;
    common::scoped_ptr<hfile::DataIndex> index_;
    std::map<std::string, std::string> file_info_meta_;
    std::string first_key_;
    bool is_first_key_;
    int entry_count_;
    int64_t total_bytes_;
    int64_t index_offset_;
    int index_count_;
    // Used calculating average key and value lengths.
    int64_t key_length_;
    int64_t value_length_;
    int64_t file_info_offset_;
    std::string last_key_;
};

}  // namespace common

#endif  // COMMON_STORAGE_SSTABLE_WRITER_UNSORTED_SSTABLE_WRITER_H
