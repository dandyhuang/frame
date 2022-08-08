cc_binary(
    name = 'libcfg-test',
    srcs = [
        'test_cfg_main.cpp'
    ],
    deps = [
        ':libcfg',
        '//thirdparty/monitor_sdk_common/net/http/server:server'
    ]
)

cc_library(
    name = "libcfg",
    srcs = [
        'config_constants.cpp',
        'global_client_settings.cpp',
        'config_manager.cpp',
        'config_loader.cpp',
        'config_loader_factory.cpp',
        'config_core_processor.cpp',
        'config_manager_internal.cpp',
        'config_stats.cpp',
        'http_config_loader.cpp',
        'remote_config_service.cpp',
        'config_scheduled_executor.cpp',
        'utils.cpp',
        'config_utils.cpp',
        'config_effect_info_collector.cpp',
        'config_effect_info.cpp',
        'local_properties.cpp'
    ],
    incs = [],
    deps = [
        '//thirdparty/monitor_sdk_common/system/threading:_thread',
        '//thirdparty/monitor_sdk_common/system/threading:_this_thread',
        '//thirdparty/monitor_sdk_common/system/threading:threading',
        '//thirdparty/monitor_sdk_common/net/http:client',
        '//thirdparty/monitor_sdk_common/storage/file:file',
        '//thirdparty/jsoncpp:jsoncpp',
        '//thirdparty/monitor_sdk_common/encoding:encoding',
        '//thirdparty/cryptopp:cryptopp',
    ],
    extra_cppflags = [
        '-std=c++11',
        '-fno-strict-aliasing',
    ],
)
