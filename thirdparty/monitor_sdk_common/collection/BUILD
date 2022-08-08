cc_library(
    name = 'memory_cache',
    deps = [
        '//thirdparty/monitor_sdk_common/system/concurrency:concurrency',
        '//thirdparty/monitor_sdk_common/system/time:time'
    ]
)

cc_test(
    name = 'memory_cache_test',
    srcs = [
        'memory_cache_test.cpp'
    ],
    deps = [
        ':memory_cache',
    ],
    heap_check = 'strict'
)

cc_test(
    name = 'prefix_map_test',
    srcs = 'prefix_map_test.cpp',
    deps = [
        '//thirdparty/monitor_sdk_common/crypto/hash:hash',
        '//thirdparty/monitor_sdk_common/base/string:string',
    ]
)

cc_library(
    name = 'bloom_filter',
    srcs = [],
    deps = [
        '//thirdparty/monitor_sdk_common/base/string:string',
        '//thirdparty/monitor_sdk_common/crypto/hash:hash',
        '//thirdparty/monitor_sdk_common/base:uint32_divisor',
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
