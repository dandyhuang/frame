cc_library(
    name = 'aioframe',
    srcs = [
        'aioframe.cpp',
        'work_thread.cpp',
    ],
    deps = [
        '//thirdparty/monitor_sdk_common/system/concurrency:concurrency',
        '//thirdparty/glog:glog'
    ],
    export_incs = [ '../../../libaio/src' ],
    incs = [ '../../../libaio/src' ],
)

cc_test(
    name = 'aio_test',
    srcs = ['aio_test.cpp'],
    deps = [':aioframe']
)

