cc_binary(
    name = 'pc-test',
    srcs = [
        'test_pc.cpp',
    ],
    deps = [
        ':librmq',
    ],
    extra_cppflags = [
        '-std=c++11',
        '-fno-strict-aliasing',
        '-Wnon-virtual-dtor',
        '-Wno-overloaded-virtual',
    ],
)

cc_binary(
    name = 'producer-client-test',
    srcs = [
        'test_producer_client.cpp',
    ],
    deps = [
        ':librmq',
    ],
    extra_cppflags = [
        '-std=c++11',
        '-fno-strict-aliasing',
        '-Wnon-virtual-dtor',
        '-Wno-overloaded-virtual',
    ],
)


cc_binary(
    name = 'consumer-client-test',
    srcs = [
        'test_consumer_client.cpp',
    ],
    deps = [
        ':librmq',
    ],
    extra_cppflags = [
        '-std=c++11',
        '-fno-strict-aliasing',
        '-Wnon-virtual-dtor',
        '-Wno-overloaded-virtual',
    ],
)


cc_library(
    name = "librmq",
    srcs = [
        'rmq_auth_request.cpp',
        'rmq_connection_config.cpp',
        'rmq_client.cpp',
        'rmq_metric.cpp',
        'rmq_remote_service.cpp',
        'rmq_scheduled_executor.cpp',
        'utils.cpp',
        'rmq_constants.cpp',
        'rmq_vhost_client.cpp',
        'rmq_consumer_client.cpp',
        'rmq_producer_client.cpp'
    ],
    incs = [],
    deps = [
        '//thirdparty/monitor_sdk_common/system/threading:_thread',
        '//thirdparty/monitor_sdk_common/system/threading:_this_thread',
        '//thirdparty/monitor_sdk_common/system/threading:threading',
        '//thirdparty/monitor_sdk_common/net/http:client',
        '//thirdparty/monitor_sdk_common/storage/file:file',
        '//thirdparty/monitor_sdk_common/crypto/uuid:uuid',
        '//thirdparty/libev:ev',
        '//thirdparty/amqp:amqp',
        '//thirdparty/jsoncpp:jsoncpp'
    ],
    extra_cppflags = [
        '-std=c++11',
        '-fno-strict-aliasing',
        '-Wnon-virtual-dtor',
        '-Wno-overloaded-virtual',
    ],
)


cc_test(
    name = 'rmq_connection_config_test',
    srcs = 'rmq_connection_config_test.cpp',
    deps = [':librmq'],
    extra_cppflags = [
        '-std=c++11',
        '-fno-strict-aliasing',
        '-Wnon-virtual-dtor',
        '-Wno-overloaded-virtual',
    ],
)
