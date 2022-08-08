cc_library(
    name = 'container',
    deps = [
        ':bitmap',
        ':bloom_filter',
    ]
)

cc_library(
    name = 'bitmap',
    srcs = 'bitmap.cpp',
    deps = [
        '//thirdparty/monitor_sdk_common/base/string:string',
    ]
)

cc_test(
    name = 'bitmap_test',
    srcs = 'bitmap_test.cpp',
    deps = ':bitmap',
)

cc_library(
    name = 'bloom_filter',
    srcs = 'bloom_filter.cpp',
    deps = [
        '//thirdparty/monitor_sdk_common/base/string:string',
        '//thirdparty/monitor_sdk_common/base/:uint32_divisor',
        '//thirdparty/monitor_sdk_common/hash:hash',
    ]
)

cc_test(
    name = 'bloom_filter_test',
    srcs = [
        'bloom_filter_test.cpp'
    ],
    deps = [
        ':bloom_filter',
        '//thirdparty/monitor_sdk_common/system/time:time'
    ]
)

cc_test(
    name = 'skiplist_test',
    srcs = 'skiplist_test.cpp',
    deps = [
        '//thirdparty/monitor_sdk_common/base:arena',
        '//thirdparty/monitor_sdk_common/base:random',
        '//thirdparty/monitor_sdk_common/hash:hash',
    ]
)

cc_test(
    name = 'lru_cache_test',
    srcs = ['lru_cache_test.cpp'],
    deps = [
        '//thirdparty/monitor_sdk_common/system/threading:threading',
        '//thirdparty/monitor_sdk_common/system/time:time',
    ],
)

