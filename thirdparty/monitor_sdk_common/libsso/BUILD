cc_library(
    name = 'libsso',
    srcs = [
        'sso_client.cpp',
        'http_invoker.cpp',
        'crypto_aes.cpp',
        'utils.cpp',
        'config_params.cpp',
    ],
    deps = [
        '//thirdparty/monitor_sdk_common/net/uri:url',
        '//thirdparty/monitor_sdk_common/net/http:client',
        '//thirdparty/cryptopp:cryptopp',
        '//thirdparty/rapidjson:rapidjson',
    ]
)

cc_binary(
    name = 'sso-test',
    srcs = [
        'sso_client_test.cpp',
    ],
    deps = [
        ':libsso',
    ]
)

cc_binary(
    name = 'pressure-test',
    srcs = [
        'pressure_test.cpp',
    ],
    deps = [
        ':libsso',
    ]
)

cc_binary(
    name = 'utils-test',
    srcs = [
        'utils_test.cpp',
        'utils.cpp',
        'config_params.cpp',
    ],
    deps = [
        '//thirdparty/monitor_sdk_common/base/string:string',
        '//thirdparty/rapidjson:rapidjson',
    ]
)