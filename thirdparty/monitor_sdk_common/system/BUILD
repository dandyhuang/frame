cc_library(
    name = 'check_error',
    srcs = 'check_error.cpp',
    deps = '//thirdparty/glog:glog',
)

cc_library(
    name = 'info',
    srcs = 'system_information.cpp',
)

cc_library(
    name = 'system',
    srcs = [
        'process.cpp',
        'dynamic_library.cpp',
    ],
    deps = [
        ':info',
        '//thirdparty/monitor_sdk_common/base/string/:string',
        '//thirdparty/monitor_sdk_common/storage/file:file_stream',
        '//thirdparty/monitor_sdk_common/system/io:io',
        '//thirdparty/monitor_sdk_common/system/time:time',
        '//thirdparty/monitor_sdk_common/system/net:ip_address',
        '//thirdparty/glog/:glog',
        '#dl'
    ]
)

cc_test(
    name = 'process_test',
    srcs = 'process_test.cpp',
    deps = [
		'//thirdparty/glog/:glog',
        '//thirdparty/monitor_sdk_common/system/concurrency/:concurrency',
        ':system'
    ]
)

cc_test(
    name = 'dynamic_library_test',
    srcs = 'dynamic_library_test.cpp',
    deps = [ 
        ':system'
    ]   
)

cc_test(
    name = 'system_information_test',
    srcs = 'system_information_test.cpp',
    deps = [
        ':info'
    ]
)
