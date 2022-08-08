cc_library(
    name = "libtrace",
    srcs = [
        'trace_manager.cpp',
        'trace_metric.cpp',
        'trace_reporter.cpp',
        'trace_scheduled_executor.cpp',
        'trace_span.cpp',
        'trace_thrift.cpp',
        'trace_tracer.cpp',
        'trace_util.cpp',
    ],
    incs = [
        '../../thirdparty/thrift',
    ],
    deps = [
        '//thirdparty/monitor_sdk_common/system/net:ip_address',
        '//thirdparty/monitor_sdk_common/system/threading:_thread',
        '//thirdparty/monitor_sdk_common/system/threading:_this_thread',
        '//thirdparty/monitor_sdk_common/system/threading:threading',
        '//thirdparty/thrift:thrift',
        '//thirdparty/protobuf:protobuf',
    ],
    extra_cppflags = [
        '-std=c++11',
        '-fno-strict-aliasing',
    ],
)


cc_binary(
    name = 'libtrace_test',
    srcs = [
        'libtrace_test.cpp',
    ],
    incs = [
        '../../thirdparty/thrift',
    ],
    deps = [
        ':libtrace',
    ]
)
