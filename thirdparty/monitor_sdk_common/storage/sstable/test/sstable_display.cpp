// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "thirdparty/monitor_sdk_common/base/string/format.h"
#include "thirdparty/monitor_sdk_common/storage/sstable/sstable_reader.h"
#include "thirdparty/monitor_sdk_common/storage/sstable/sstable_writer.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gflags/gflags.h"

DEFINE_string(sstable_path,
              "common/storage/sstable/test/testdata/test.sstable",
              "path of sstable");

int main(int argc, char** argv) {
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    common::scoped_ptr<common::SSTableReader> sstable(
                    common::SSTableReader::Open(FLAGS_sstable_path, common::SSTableReader::ON_DISK));
    CHECK(sstable.get());

    LOG(INFO)<< "testing NewIterator ...";
    common::scoped_ptr<common::SSTableReader::Iterator> iter(sstable->NewIterator());
    CHECK(iter.get());
    int i = 0;
    do {
        ++i;
        LOG(INFO)<< "key:" << iter->key();
        LOG(INFO)<< "value:" << iter->value();
        iter->Next();
    } while (iter->Valid());
}
