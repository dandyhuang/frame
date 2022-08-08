cc_library(
    name = "libcurl_wrapper",
    srcs = [
        'libcurl_wrapper.cpp',
    ],
    incs = '../../thirdparty/curl/include',
    export_incs = '../../thirdparty/curl/include',
    deps = [
        "//thirdparty/monitor_sdk_common/base/string:string",
        "//thirdparty/glog:glog",
        "//thirdparty/curl:curl",
    ],
)

cc_test(
    name = "libcurl_wrapper_test",
    srcs = [
        'libcurl_wrapper_test.cpp',
    ],
    incs = '../../thirdparty/curl/include',
    deps = [
        ":libcurl_wrapper",
        "//thirdparty/gtest:gtest",
        "//thirdparty/perftools:tcmalloc",
    ],
)