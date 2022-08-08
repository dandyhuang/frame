cc_binary(
    name = 'test-main',
    srcs = [
        'test_main.cpp'
    ],
    deps = [
        ':libhawking'
    ],
)

cc_library(
    name = 'libhawking',
    srcs = [
        'device_uid_provider.cpp',
        'executor/app_client_strategy_executor.cpp',
        'executor/app_version_strategy_executor.cpp',
        'executor/device_uid_hash_strategy_executor.cpp',
        'executor/imei_tail_strategy_executor.cpp',
        'executor/open_id_hash_strategy_executor.cpp',
        'gray_test_cache_manager.cpp',
        'gray_test_config.cpp',
        'gray_test_log_utils.cpp',
        'gray_test_query_client.cpp',
        'gray_test_query_service.cpp',
        'gray_test_scheduled_executor.cpp',
        'hash_code_utils.cpp',
        'ret_code.cpp',
        'strategy_executor_manager.cpp',
    ],
    incs = [],
    deps = [
        '//thirdparty/monitor_sdk_common/base/string:_algorithm',
        '//thirdparty/monitor_sdk_common/crypto/hash:md5',
        '//thirdparty/monitor_sdk_common/crypto/hash:sha1',
        '//thirdparty/monitor_sdk_common/net/http:client',
        '//thirdparty/monitor_sdk_common/system/threading:_thread',
        '//thirdparty/monitor_sdk_common/system/threading:_this_thread',
        '//thirdparty/monitor_sdk_common/system/threading:threading',
        '//thirdparty/jsoncpp:jsoncpp',
    ],
    extra_cppflags = [
        '-std=c++11',
        '-fno-strict-aliasing',
    ],
)

cc_test(
    name = 'gray-test-query-client-test',
    srcs = ['test_gray_test_query_client.cpp'],
    deps = [
        ':libhawking',
    ],
    extra_cppflags = [
        '-std=c++11',
    ],
)

cc_test(
    name = 'app-client-strategy-executor-test',
    srcs = ['executor/app_client_strategy_executor_test.cpp'],
    deps = [
        ':libhawking',
    ],
    extra_cppflags = [
        '-std=c++11',
    ],
)

cc_test(
    name = 'app-version-strategy-executor-test',
    srcs = ['executor/app_version_strategy_executor_test.cpp'],
    deps = [
        ':libhawking',
    ],
    extra_cppflags = [
        '-std=c++11',
    ],
)

cc_test(
    name = 'device-uid-hash-strategy-executor-test',
    srcs = ['executor/device_uid_hash_strategy_executor_test.cpp'],
    deps = [
        ':libhawking',
    ],
    extra_cppflags = [
        '-std=c++11',
    ],
)

cc_test(
    name = 'imei-tail-strategy-executor-test',
    srcs = ['executor/imei_tail_strategy_executor_test.cpp'],
    deps = [
        ':libhawking',
    ],
    extra_cppflags = [
        '-std=c++11',
    ],
)

cc_test(
    name = 'open-id-hash-strategy-executor-test',
    srcs = ['executor/open_id_hash_strategy_executor_test.cpp'],
    deps = [
        ':libhawking',
    ],
    extra_cppflags = [
        '-std=c++11',
    ],
)