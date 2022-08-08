cc_library(
    name = 'hash',
    srcs = [
        'crc.cpp',
        'md4.cpp',
        'md5.cpp',
        'md5_2.cpp',
        'string_hashes.cpp',
        'city.cpp',
        'fingerprint.cpp'
    ],
    deps = [
        '//thirdparty/monitor_sdk_common/encoding:encoding',
       '//thirdparty/openssl:crypto',
        '//thirdparty/monitor_sdk_common/crypto/hash/murmur:murmur',
        '//thirdparty/monitor_sdk_common/crypto/hash/fnv:fnv',
    ]
)

cc_test(
    name = 'city_test',
    srcs = 'city_test.cpp',
    deps = ':hash'
)

cc_test(
    name = 'crc_test',
    srcs = 'crc_test.cpp',
    deps = ':hash'
)

cc_test(
    name = 'fingerprint_test',
    srcs = 'fingerprint_test.cpp',
    deps = ':hash'
)

cc_test(
    name = 'md4_test',
    srcs = 'md4_test.cpp',
    deps = ':hash'
)


cc_library(
    name = 'md5',
    srcs = 'md5.cpp',
    deps = [
        '//thirdparty/monitor_sdk_common/base:int128',
        '//thirdparty/monitor_sdk_common/base/string:string',
        '//thirdparty/monitor_sdk_common/encoding:encoding',
    ],
)

cc_test(
    name = 'md5_test',
    srcs = 'md5_test.cpp',
    deps = [':md5'],
)

cc_test(
    name = 'md5_test_2',
    srcs = 'md5_test_2.cpp',
    deps = [
        ':hash',
        '//thirdparty/monitor_sdk_common/base/string:string',
    ],
)

cc_library(
    name = 'sha1',
    srcs = 'sha1.cpp',
    deps = [
        '//thirdparty/monitor_sdk_common/encoding:encoding',
    ],
)

cc_test(
    name = 'sha1_test',
    srcs = 'sha1_test.cpp',
    deps = [
        ':sha1',
    ],
)
