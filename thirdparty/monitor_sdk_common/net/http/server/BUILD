cc_library(
    name = 'server',
    srcs = [
        'connection.cpp',
        'handler.cpp',
        'server.cpp',
    ],
    deps = [
        '//thirdparty/monitor_sdk_common/net/http:types',
        '//thirdparty/monitor_sdk_common/net/uri:url',
        '//thirdparty/monitor_sdk_common/system/event_dispatcher:event_dispatcher',
        '//thirdparty/monitor_sdk_common/system/net:net',
        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog',
    ],
)

cc_binary(
    name = 'example_server',
    srcs = 'example.cpp',
    deps = ':server'
)

cc_test(
    name = 'server_test',
    srcs = 'server_test.cpp',
    deps = ':server'
)


