cc_library(
    name = 'oss',        
    srcs = 'oss_api.cpp',
    deps = '//thirdparty/monitor_sdk_common/base:shmutil',
)

cc_binary(
    name = 'oss_test',
    srcs = 'oss_test.cpp',
    deps = [
        ':oss',
        '#pthread',
    ],
    extra_cppflags = [
        '-std=c++11',
    ],
)
