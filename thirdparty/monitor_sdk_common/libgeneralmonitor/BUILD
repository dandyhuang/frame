cc_library(
    name = "libgeneralmonitor",
    srcs = [
        'util/util.cpp',
        'util/tc_base64.cpp',
        'context/monitor_context.cpp',
        'monitor_config/monitor_config.cpp',
        'general_monitor_report.cpp',
        'thread/calc_thread.cpp',
        'thread/report_thread.cpp',
    ],
    deps = [
        '//thirdparty/monitor_sdk_common/net/http:client',
        '//thirdparty/monitor_sdk_common/libbeesbus:libbeesbus',
        '//thirdparty/monitor_sdk_common/crypto/hash/murmur:murmur',
        '//thirdparty/rapidjson:rapidjson',
        '//thirdparty/glog:glog',
        '//thirdparty/boost:boost',
        '//thirdparty/datasketches:datasketches_kll',
    ],
    extra_cppflags = [
        '-std=c++11',
        '-fno-strict-aliasing',
    ],
)

cc_binary(
    name = 'generalmonitorexample',
    srcs = 'example/general_monitor_example.cpp',
    deps = [
        ':libgeneralmonitor',
    ],
    extra_cppflags = [
        '-std=c++11',
    ],
)