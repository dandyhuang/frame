cc_binary(
	name = 'vep_client_test',
	srcs = [
		'vep_client_test.cpp',
	],
	deps = [
	    ':libvep',
	],
)

cc_binary(
	name = 'vep_thirdparty_client_test',
	srcs = [
		'vep_thirdparty_client_test.cpp',
	],
	deps = [
	    ':libvep',
	],
)

cc_binary(
	name = 'vep_scene_client_test',
	srcs = [
		'vep_scene_client_test.cpp',
	],
	deps = [
	    ':libvep',
	],
)

cc_library(
    name = 'libvep',
    	srcs = [
    	    'vep_client.cpp',
    	    'vep_scene_related_client.cpp',
    	    'vep_thirdparty_related_client.cpp',
    		'vep_scheduled_executor.cpp',
    		'./cache/cache_utils.cpp',
    		'./cache/layer_cache_key_generator.cpp',
    		'./config/constants.cpp',
    		'./config/dynamic_client_config.cpp',
    		'./provider/vep_config_provider.cpp',
    		'./provider/vep_layer_config_provider.cpp',
    		'./provider/http/http_invoker.cpp',
    		'./provider/service/vep_config_service.cpp',
    		'./provider/service/client_report_service.cpp',
    		'./provider/service/layer/layer_config_service.cpp',
            './util/utils.cpp',
            './util/converter_utils.cpp',
            './util/deserialize_utils.cpp',
            './util/log_utils.cpp',
    	],
        incs = '../../thirdparty/curl/include',
    	deps = [
    	    '//thirdparty/monitor_sdk_common/base/string:string',
    	    '//thirdparty/monitor_sdk_common/crypto/hash:md5',
    	    '//thirdparty/monitor_sdk_common/net/http:client',
    	    '//thirdparty/monitor_sdk_common/storage/file:file',
    	    '//thirdparty/monitor_sdk_common/system/threading:_thread_pool',
    		'//thirdparty/jsoncpp:jsoncpp',
    		'//thirdparty/gflags:gflags',
            ':libcurl_wrapper',
    	],
    	extra_cppflags = [
            '-std=c++11',
        ]
)

cc_test(
	name = 'client_fct',
	srcs = [
		'./test/client_fct_test.cpp',
	],
	deps = [
	    ':libvep',
	],
)

cc_test(
	name = 'thirdparty_client_fct',
	srcs = [
		'./test/thirdparty_client_fct_test.cpp',
	],
	deps = [
	    ':libvep',
	],
)

cc_test(
	name = 'scene_client_fct',
	srcs = [
		'./test/scene_client_fct_test.cpp',
	],
	deps = [
	    ':libvep',
	],
)

cc_binary(
	name = 'pressure_test',
	srcs = [
		'./test/pressure_test.cpp',
	],
	deps = [
	    ':libvep',
	],
)

cc_binary(
	name = 'thirdparty_client_unit_test',
	srcs = [
		'./test/thirdparty_client_unit_test.cpp',
	],
	deps = [
	    ':libvep',
	],
)

cc_binary(
	name = 'scene_client_unit_test',
	srcs = [
		'./test/scene_client_unit_test.cpp',
	],
	deps = [
	    ':libvep',
	],
)

cc_binary(
	name = 'client_unit_test',
	srcs = [
		'./test/client_unit_test.cpp',
	],
	deps = [
	    ':libvep',
	],
)

cc_library(
	name = 'util',
	srcs = [
	    './util/utils.cpp',
	    './util/log_utils.cpp',
	],
	deps = [
	    '//thirdparty/monitor_sdk_common/crypto/hash:md5',
	    '//thirdparty/monitor_sdk_common/base/string:string',
	    '//thirdparty/monitor_sdk_common/net/http:client',
	],
)

cc_library(
	name = 'dynamic_config',
	srcs = [
	    './config/dynamic_client_config.cpp',
	    './provider/http/http_invoker.cpp',
	    './util/deserialize_utils.cpp',
	    'vep_scheduled_executor.cpp',
	],
	deps = [
	    ':util',
	    '//thirdparty/monitor_sdk_common/system/threading:_thread_pool',
	    '//thirdparty/monitor_sdk_common/crypto/hash:md5',
	    '//thirdparty/monitor_sdk_common/base/string:string',
	    '//thirdparty/monitor_sdk_common/net/http:client',
	],
)

cc_test(
	name = 'http_test',
	srcs = [
		'./provider/http/http_invoker_test.cpp',
		'./provider/http/http_invoker.cpp',
		'./config/constants.cpp',
	],
	deps = [
	    ':util',
	    '//thirdparty/monitor_sdk_common/net/http:client',
		'//thirdparty/jsoncpp:jsoncpp',
	],
)

cc_test(
	name = 'global_initialize_test',
	srcs = [
		'./provider/http/global_initialize_test.cpp',
	],
	deps = [
	],
)

cc_library(
    name = "libcurl_wrapper",
    srcs = [
        './provider/http/libcurl_wrapper.cpp',
    ],
    incs = '../../thirdparty/curl/include',
    deps = [
        "//thirdparty/monitor_sdk_common/base/string:string",
        "//thirdparty/glog:glog",
        "//thirdparty/curl:curl",
    ],
)

cc_test(
    name = "libcurl_wrapper_test",
    srcs = [
        './provider/http/libcurl_wrapper_test.cpp',
    ],
    incs = '../../thirdparty/curl/include',
    deps = [
        ":libcurl_wrapper",
        "//thirdparty/gtest:gtest",
        "//thirdparty/perftools:tcmalloc",
    ],
)

cc_test(
    name = 'deserialize_utils_test',
    srcs = [
    		'./util/deserialize_utils_test.cpp',
    		'./util/deserialize_utils.cpp',
    		'./config/constants.cpp',
    ],
    deps = [
            ':util',
    	    '//thirdparty/monitor_sdk_common/base/string:string',
    		'//thirdparty/jsoncpp:jsoncpp',
    ],
)

cc_test(
    name = 'layer_report_param_test',
    srcs = [
    		'./request/layer_report_param_test.cpp',
    		'./config/constants.cpp',
    ],
    deps = [
            ':util',
    		'//thirdparty/jsoncpp:jsoncpp',
    ],
)

cc_test(
    name = 'layer_config_service_test',
    srcs = [
            './provider/http/http_invoker.cpp',
            './config/constants.cpp',
            './util/deserialize_utils.cpp',
    		'./provider/service/vep_config_service.cpp',
    		'./provider/service/layer/layer_config_service.cpp',
    		'./provider/service/layer/layer_config_service_test.cpp',
    ],
    incs = '../../thirdparty/curl/include',
    deps = [
            ':util',
            ':dynamic_config',
            ':libcurl_wrapper',
            '//thirdparty/monitor_sdk_common/net/http:client',
    		'//thirdparty/jsoncpp:jsoncpp',
    ],
)

cc_test(
    name = 'vep_config_service_test',
    srcs = [
            './provider/service/vep_config_service_test.cpp',
            './provider/service/vep_config_service.cpp',
            './provider/service/client_report_service.cpp',
            './provider/http/http_invoker.cpp',
            './config/constants.cpp',
            './cache/cache_utils.cpp',
            './util/deserialize_utils.cpp',
    ],
    deps = [
            ':util',
            ':dynamic_config',
            ':libcurl_wrapper',
            '//thirdparty/monitor_sdk_common/net/http:client',
    		'//thirdparty/jsoncpp:jsoncpp',
    ],
)

cc_test(
    name = 'converter_test',
    srcs = [
            './util/converter_utils_test.cpp',
            './util/converter_utils.cpp',
            './util/deserialize_utils.cpp',
    		'./provider/service/vep_config_service.cpp',
    		'./provider/service/layer/layer_config_service.cpp',
    		'./provider/http/http_invoker.cpp',
            './config/constants.cpp',
    ],
    incs = '../../thirdparty/curl/include',
    deps = [
            ':util',
            ':dynamic_config',
            ':libcurl_wrapper',
            '//thirdparty/monitor_sdk_common/base/string:string',
            '//thirdparty/monitor_sdk_common/net/http:client',
    		'//thirdparty/jsoncpp:jsoncpp',
    ],
)

cc_test(
    name = 'layer_cache_test',
    srcs = [
            './cache/cache_utils_test.cpp',
            './cache/cache_utils.cpp',
            './util/converter_utils.cpp',
            './util/deserialize_utils.cpp',
            './config/constants.cpp',
            './provider/http/http_invoker.cpp',
    		'./provider/service/vep_config_service.cpp',
    		'./provider/service/client_report_service.cpp',
    		'./provider/service/layer/layer_config_service.cpp',
    ],
    incs = '../../thirdparty/curl/include',
    deps = [
            ':util',
            ':dynamic_config',
            ':libcurl_wrapper',
            '//thirdparty/monitor_sdk_common/base/string:string',
            '//thirdparty/monitor_sdk_common/net/http:client',
    		'//thirdparty/jsoncpp:jsoncpp',
    ],
)
