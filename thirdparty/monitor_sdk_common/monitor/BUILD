cc_library(
	name = 'monitor_api',
	srcs = [
		'monitor_api.cpp',
	],
	deps = [
        '#pthread',
        '//thirdparty/monitor_sdk_common/system/net:ip_address',
        '//thirdparty/monitor_sdk_common/net/http:client',
	],
	extra_cppflags = ['-Wno-unused-variable', '-std=c++11'],
)

cc_binary(
	name = 'monitor_api_test',
	srcs = [
		'monitor_api_test.cpp',
	],
	deps = [
        ':monitor_api',
	],
	extra_cppflags = ['-Wno-unused-variable', '-std=c++11'],
)

