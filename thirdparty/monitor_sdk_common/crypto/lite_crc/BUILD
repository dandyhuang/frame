cc_library(
    name = 'lite_crc',
    srcs = [
        'fast_crc.cc'
    ],
    always_optimize = 1
)

cc_test(
    name = 'lite_crc_test',
    srcs = [
        'fast_crc_test.cc'
    ],
    deps = [
        ':lite_crc',
        '//thirdparty/glog:glog'
    ]
)
