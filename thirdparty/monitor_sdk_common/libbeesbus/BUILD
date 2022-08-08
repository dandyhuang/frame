cc_library(
    name = "libbeesbus",
    srcs = [
        'thrift/bus_constants.cpp',
        'thrift/bus_types.cpp',
        'thrift/ThriftSourceProtocol.cpp',
        'report/report_to_bees.cpp',
        'bees_bus_report.cpp',
        'context/bees_context.cpp',
        'client/thrift_client.cpp',
        'log/glog_helper.cpp',
    ],
    export_incs = [
        '../../../thirdparty/thrift',
    ],
    deps = [
        '//thirdparty/monitor_sdk_common/net/http:client',
        '//thirdparty/thrift-0.13.0/lib/cpp/src/thrift:thrift',
        '//thirdparty/rapidjson:rapidjson',
        '//thirdparty/glog:glog',
    ],
    extra_cppflags = [
        '-std=c++11',
        '-fno-strict-aliasing',
    ],
)

cc_binary(
    name = 'beesbusexample',
    srcs = 'example/bees_bus_example.cpp',
    deps = [
        ':libbeesbus',
    ],
    extra_cppflags = [
        '-std=c++11',
    ],
)