cc_binary(
    name = 'test-json-pb',
    srcs = [
        'test.cpp',
    ],
    deps = [
        ':test-json-pb-proto',
        ':pb-json-converter',
        '#pthread',
    ]
)

cc_library(
	name = 'pb-json-converter',
	srcs = [
		'pb_json_converter.cpp',
	],
	
	extra_cppflags = [
		'-std=c++11',
		'-Wall',
		'-Werror',
		'-Wno-deprecated',
	],
)

proto_library(
    name = 'test-json-pb-proto',
    srcs = ['test.proto'],
    plugins = ['custom']
)
