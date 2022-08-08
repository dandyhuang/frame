cc_library(
	name = 'libvivolog',
	srcs = [
		'mempool.cpp',
        'vivolock.cpp',
        'vivolog_util.cpp',
        'vivolog.cpp',
        'vivotrace.cpp'
	],
	deps = [
        '#pthread'
	],
	extra_cppflags = ['-Wno-unused-result', '-Wno-format-nonliteral']
)

cc_binary(
	name = 'test_log',
	srcs = [
		'test_vivolog.cpp',
	],
	deps = [
        ':libvivolog'
	],
	extra_cppflags = ['-Wno-unused-result', '-Wno-format-nonliteral']
)

cc_binary(
	name = 'test_trace',
	srcs = [
		'test_vivotrace.cpp',
	],
	deps = [
        ':libvivolog'
	],
	extra_cppflags = ['-Wno-unused-result', '-Wno-format-nonliteral']
)
