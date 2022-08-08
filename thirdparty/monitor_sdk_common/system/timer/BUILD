cc_library(
    name = 'timer',
    srcs = 'timer_manager.cpp',
    deps = [
        '//thirdparty/monitor_sdk_common/system/time:time',
        '//thirdparty/monitor_sdk_common/system/concurrency:concurrency',
    ]
)

cc_test(
    name = 'timer_test',
    srcs = 'timer_manager_test.cpp',
    deps = ':timer'
)
