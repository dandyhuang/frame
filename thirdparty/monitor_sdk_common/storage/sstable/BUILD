# Copyright (c) 2013, The Toft Authors. All rights reserved.
# Author: Ye Shunping <yeshunping@gmail.com>

cc_library(
    name = 'sstable',
    deps = [
        '//thirdparty/monitor_sdk_common/storage/sstable/hfile:file_trailer',
        '//thirdparty/monitor_sdk_common/storage/sstable/hfile:file_info',
        '//thirdparty/monitor_sdk_common/storage/sstable/hfile:data_index',
        '//thirdparty/monitor_sdk_common/storage/sstable/hfile:data_block',
    ],
)

cc_library(
    name = 'sstable_reader',
    deps = [
        '//thirdparty/monitor_sdk_common/storage/sstable/reader:_sstable_reader',
        '//thirdparty/monitor_sdk_common/storage/sstable/reader:_merged_sstable_reader',
    ],
)

cc_library(
    name = 'sstable_writer',
    deps = [
        '//thirdparty/monitor_sdk_common/storage/sstable/writer:single_sstable_writer',
        '//thirdparty/monitor_sdk_common/storage/sstable/writer:unsorted_sstable_writer',
        '//thirdparty/monitor_sdk_common/storage/sstable/writer:composited_sstable_writer',
        '//thirdparty/monitor_sdk_common/storage/sstable/writer:sharding_sstable_writer',
    ],
)
