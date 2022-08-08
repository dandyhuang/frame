// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef COMMON_STORAGE_SSTABLE_WRITER_COMPOSITED_SSTABLE_WRITER_H
#define COMMON_STORAGE_SSTABLE_WRITER_COMPOSITED_SSTABLE_WRITER_H

#include <map>
#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/storage/sstable/writer/base_sstable_writer.h"

namespace common {
class SingleSSTableWriter;

class CompositedSSTableWriter : public SSTableWriter {
    COMMON_DECLARE_UNCOPYABLE(CompositedSSTableWriter);

public:
    explicit CompositedSSTableWriter(const SSTableWriteOption &option);
    CompositedSSTableWriter(const SSTableWriteOption &option,
                            const int64_t total_in_memory);

    ~CompositedSSTableWriter();

    virtual bool Add(const std::string &key, const std::string &value);
    virtual void AddMetaData(const std::string &key, const std::string &value);
    virtual bool Flush();

private:
    void GetNewWriter();
    void DeleteTmpFiles();

    int64_t total_byte_;
    int64_t curr_byte_;
    std::vector<std::string> paths_;
    std::map<std::string, std::string> file_info_meta_;
    common::scoped_ptr<SingleSSTableWriter> builder_;
    const int64_t batch_write_size_;
};

}  // namespace common

#endif  // COMMON_STORAGE_SSTABLE_WRITER_COMPOSITED_SSTABLE_WRITER_H
