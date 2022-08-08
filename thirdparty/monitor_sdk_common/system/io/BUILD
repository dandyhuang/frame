cc_library(
    name = 'io',
    srcs = [
        'directory.cpp',
        'common_file.cpp',
        'file_lock.cpp',
        'textfile.cpp',
        'path.cpp',
        'mount.cpp'
    ],
    deps = [
        '//thirdparty/monitor_sdk_common/text:wildcard',
        '//thirdparty/monitor_sdk_common/base/string:string',
        '//thirdparty/glog:glog'
    ]
)

cc_test(
    name = 'path_test',
    srcs = 'path_test.cpp',
    deps = ':io'
)

cc_test(
    name = 'file_test',
    srcs = 'file_test.cpp',
    deps = ':io',
    testdata = ['test_unix.txt']
)

cc_test(
    name = 'file_lock_test',
    srcs = 'file_lock_test.cpp',
    deps = ':io'
)

cc_test(
    name = 'textfile_test',
    srcs = 'textfile_test.cpp',
    deps = ':io',
    testdata = ['test_dos.txt', 'test_unix.txt', 'xxxx']
)

cc_test(
    name = 'directory_test',
    srcs = 'directory_test.cpp',
    deps = ':io'
)

cc_test(
    name='mount_test',
    srcs=[
        'mount_test.cpp'
    ],
    deps=[
        ':io'
    ],
    heap_check='strict'
)
