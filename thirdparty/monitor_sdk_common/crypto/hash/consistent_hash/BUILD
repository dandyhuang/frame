cc_library(
    name = 'consistent_hash',
    srcs = [
        'consistent_hash.cpp'
    ],
    deps = [
        '//thirdparty/monitor_sdk_common/crypto/hash:hash',
        "//thirdparty/monitor_sdk_common/base/string:string"
    ]
)

cc_test(
    name = 'consistent_hash_test',
    srcs = 'consistent_hash_test.cpp',
    deps = ':consistent_hash'
)
