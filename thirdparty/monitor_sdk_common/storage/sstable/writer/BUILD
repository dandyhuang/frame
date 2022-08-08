# Copyright (c) 2013, The Toft Authors. All rights reserved.
# Author: Ye Shunping <yeshunping@gmail.com>

cc_library(
    name = 'base_sstable_writer',
    srcs = 'base_sstable_writer.cpp',
    deps = [
        '//thirdparty/monitor_sdk_common/hash:hash',
        '//thirdparty/monitor_sdk_common/storage/file:file',
        '//thirdparty/monitor_sdk_common/storage/path:path',
        '//thirdparty/monitor_sdk_common/compress/block:block',
    ],
)

cc_library(
    name = 'single_sstable_writer',
    srcs = 'single_sstable_writer.cpp',
    deps = [
        ':base_sstable_writer',
        '//thirdparty/monitor_sdk_common/storage/sstable:sstable',
    ],
)

cc_library(
    name = 'unsorted_sstable_writer',
    srcs = 'unsorted_sstable_writer.cpp',
    deps = [
        ':base_sstable_writer',
        '//thirdparty/monitor_sdk_common/storage/sstable:sstable',
    ],
)

cc_library(
    name = 'composited_sstable_writer',
    srcs = 'composited_sstable_writer.cpp',
    deps = [
        ':base_sstable_writer',
        ':single_sstable_writer',
        ':unsorted_sstable_writer',
        '//thirdparty/monitor_sdk_common/storage/sstable:sstable',
        '//thirdparty/monitor_sdk_common/storage/sstable:sstable_reader',
    ],
)

cc_library(
    name = 'sharding_sstable_writer',
    srcs = 'sharding_sstable_writer.cpp',
    deps = [
        ':base_sstable_writer',
        ':composited_sstable_writer',
        '//thirdparty/monitor_sdk_common/storage/sstable:sstable',
        '//thirdparty/monitor_sdk_common/storage/sstable:sstable_reader',
        '//thirdparty/monitor_sdk_common/storage/sharding:sharding',
    ],
)