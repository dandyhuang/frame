cc_library(
    name = 'module_a',
    srcs = 'module_a.cpp',
    deps = '//thirdparty/monitor_sdk_common/base:module',
    link_all_symbols = 1
)

cc_library(
    name = 'module_b',
    srcs = 'module_b.cpp',
    deps = [':module_a'],
    link_all_symbols = 1
)

cc_test(
    name = 'module_test',
    srcs = 'module_test.cpp',
    deps = ':module_b'
)

cc_test(
    name = 'dup_test',
    srcs = 'dup_test.cpp',
    deps = '//thirdparty/monitor_sdk_common/base:module',
)

cc_test(
    name = 'cycle_test',
    srcs = [
        'cycle_test.cpp',
        'cycle_test_helper1.cpp',
        'cycle_test_helper2.cpp',
        'cycle_test_helper3.cpp',
        'cycle_test_helper4.cpp',
        'cycle_test_helper5.cpp',
    ],
    deps = '//thirdparty/monitor_sdk_common/base:module',
)

cc_test(
    name = 'dependency_map_test',
    srcs = 'dependency_map_test.cpp',
    deps = '//thirdparty/monitor_sdk_common/base:module'
)
