cc_library(
    name = 'process',
    srcs = [
        'process_local_const.cpp',
        'sub_process.cpp',
        'this_process.cpp',
    ],
    deps = [
        '//thirdparty/monitor_sdk_common/encoding:encoding',
        '//thirdparty/monitor_sdk_common/storage/file:file',
        '//thirdparty/monitor_sdk_common/system/threading:_this_thread',
        '//thirdparty/monitor_sdk_common/system/time:time',
    ]
)

cc_test(
    name = 'process_local_const_test',
    srcs = 'process_local_const_test.cpp',
    deps = ':process'
)

cc_test(
    name = 'sub_process_test',
    srcs = 'sub_process_test.cpp',
    deps = [
        ':process'
    ]
)

cc_test(
    name = 'this_process_test',
    srcs = 'this_process_test.cpp',
    deps = [
        ':process'
    ]
)
