// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef COMMON_STORAGE_SSTABLE_READER_IN_MEMORY_SSTABLE_READER_H
#define COMMON_STORAGE_SSTABLE_READER_IN_MEMORY_SSTABLE_READER_H

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/storage/sstable/sstable_reader.h"

namespace common {
namespace hfile {
class DataBlock;
} // namespace hfile

typedef std::vector<std::pair<std::string, std::vector<std::string> > > DataVector;

class InMemorySSTableReader : public SSTableReader {
    COMMON_DECLARE_UNCOPYABLE(InMemorySSTableReader);
public:
    InMemorySSTableReader();
    ~InMemorySSTableReader();

    virtual void Init();

    virtual Iterator *Seek(const std::string &key);

private:
    friend class InMemoryIterator;

    common::scoped_ptr<hfile::DataBlock> cached_block_;
    //  Replace with hash_map
    std::map<std::string, DataVector::iterator> index_;
    DataVector data_;
};

class InMemoryIterator : public SSTableReader::Iterator {
    COMMON_DECLARE_UNCOPYABLE(InMemoryIterator);

public:
    InMemoryIterator(InMemorySSTableReader *sstable, const std::string &key);
    ~InMemoryIterator();

    void SeekKey(const std::string &key);
    virtual void Next();

private:
    bool NextItem();
    void LoadItem();

    InMemorySSTableReader *sstable_;
    DataVector::iterator cur_it_;
    int pos_;
    int size_;
};

}  // namespace common

#endif  // COMMON_STORAGE_SSTABLE_READER_IN_MEMORY_SSTABLE_READER_H
