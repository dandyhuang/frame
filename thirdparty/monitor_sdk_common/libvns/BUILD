cc_library(
    name = 'vns',
    srcs = [
        'src/common_params.cpp',
        'src/constants.cpp',
        'src/scheduled/scheduled_executor.cpp',
        'src/naming_service.cpp',
        'src/naming_proxy.cpp',
        'src/beat/beat_info.cpp',
        'src/beat/beat_reactor.cpp',
        'src/utils/utilandcoms.cpp',
        'src/utils/log_utils.cpp',
        'src/utils/utils.cpp',
        'src/utils/json_utils.cpp',
        'src/cache/disk_cache.cpp',
        'src/core/host_reactor.cpp',
        'src/core/push_receiver.cpp',
        'src/object/instance.cpp',
        'src/object/service_info.cpp',
        'src/object/token_info.cpp',
        'src/backups/failover_reactor.cpp',
    ],
    incs = ['include'],
    export_incs = "include",
    deps = [
        '//thirdparty/monitor_sdk_common/base/string:string',
        '//thirdparty/monitor_sdk_common/net/http:client',
        '//thirdparty/monitor_sdk_common/storage/file:file',
        '//thirdparty/monitor_sdk_common/system/threading:_thread_pool',
        '//thirdparty/rapidjson:rapidjson',
    ],
    extra_cppflags = [
        '-std=c++11',
        '-fPIC',
    ],
)

cc_binary(
	name = 'NamingTest',
	srcs = [
		'test/test_naming_service.cpp',
	],
	deps = [
	    ':vns',
	    '//thirdparty/gflags:gflags',
	],
)

cc_binary(
	name = 'ClientTest',
	srcs = [
		'test/test_client.cpp',
	],
	deps = [
	    ':vns',
	    '//thirdparty/gflags:gflags',
	],
)

cc_binary(
	name = 'ServiceTest',
	srcs = [
		'test/test_service.cpp',
	],
	deps = [
	    ':vns',
	    '//thirdparty/gflags:gflags',
	],
)

cc_binary(
	name = 'ClientTest2',
	srcs = [
		'test/test_client2.cpp',
	],
	deps = [
	    ':vns',
	    '//thirdparty/gflags:gflags',
	],
)

cc_binary(
	name = 'ServiceTest2',
	srcs = [
		'test/test_service2.cpp',
	],
	deps = [
	    ':vns',
	    '//thirdparty/gflags:gflags',
	],
)

cc_binary(
	name = 'PressClientTest',
	srcs = [
		'test/test_press_client.cpp',
	],
	deps = [
	    ':vns',
	    '//thirdparty/gflags:gflags',
	],
	extra_cppflags = [
        '-g',
    ],
)

cc_binary(
	name = 'PressServiceTest',
	srcs = [
		'test/test_press_server.cpp',
	],
	deps = [
	    ':vns',
	    '//thirdparty/gflags:gflags',
	],
	extra_cppflags = [
	    '-g',
    ],
)

cc_binary(
	name = 'ProxyTest',
	srcs = [
		'test/test_naming_proxy.cpp',
	],
	deps = [
	    ':vns',
	    '//thirdparty/gflags:gflags',
	],
)

cc_binary(
	name = 'HostTest',
	srcs = [
		'test/test_host_reactor.cpp',
	],
	deps = [
	    ':vns',
	    '//thirdparty/gflags:gflags',
	],
)

cc_binary(
	name = 'BeatTest',
	srcs = [
		'test/test_beat_reactor.cpp',
	],
	deps = [
	    ':vns',
	    '//thirdparty/gflags:gflags',
	],
)

cc_binary(
	name = 'CacheTest',
	srcs = [
		'test/test_cache.cpp',
	],
	deps = [
	    ':vns',
	    '//thirdparty/gflags:gflags',
	],
)

cc_binary(
	name = 'JsonUtilsTest',
	srcs = [
		'test/test_json_utils.cpp',
	],
	deps = [
	    ':vns',
	    '//thirdparty/gflags:gflags',
	],
)

cc_binary(
	name = 'AsynQueueTest',
	srcs = [
		'test/test_asyn_queue.cpp',
	],
	deps = [
	    ':vns',
	    '//thirdparty/gflags:gflags',
	],
)