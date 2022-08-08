cc_library(
    name = 'file',
    srcs = [
        'file.cpp',
        'local_file.cpp',
    ],
    deps = [
        '//thirdparty/monitor_sdk_common/base/string:string',
        '//thirdparty/monitor_sdk_common/base:class_registry',
        '//thirdparty/monitor_sdk_common/storage/path:path',
        '//thirdparty/monitor_sdk_common/text:wildcard'
    ],
    link_all_symbols = True
)

cc_test(
    name = 'file_test',
    srcs = 'file_test.cpp',
    deps = ':file',
    testdata = 'testdata',
)

cc_library(
    name = 'mock_file',
    srcs = 'mock_file.cpp',
    deps = [
        ':file',
        '//thirdparty/gmock:gmock',
    ],
    link_all_symbols = True,
)

cc_test(
    name = 'mock_file_test',
    srcs = 'mock_file_test.cpp',
    deps = ':mock_file',
)

'''
add files
'''
cc_library(
    name = 'file2',
    srcs = [
        'file2.cpp',
        'abstract_stream_file.cpp',
        'file_define.cpp'
    ],
    deps = [
        '//thirdparty/glog:glog',
        '//thirdparty/monitor_sdk_common/base:module',
        '//thirdparty/monitor_sdk_common/base/string:string',
        '//thirdparty/monitor_sdk_common/system:system',
        '//thirdparty/monitor_sdk_common/system/concurrency:concurrency',
        '//thirdparty/monitor_sdk_common/crypto/lite_crc:lite_crc'
    ]
)

cc_test(
    name = 'file_test2',
    srcs = [
        'file_test2.cpp',
        'abstract_stream_file_test.cpp'
    ],
    deps = [
        ':local_file2',
        '//thirdparty/monitor_sdk_common/system/io:io',
    ],
    incs = [ '../../../libaio/src' ],
)

cc_library(
    name = 'local_file2',
    srcs = [
        'local_file2.cpp'
    ],
    deps = [
        ':file2',
        '//thirdparty/monitor_sdk_common/system:system',
        '//thirdparty/monitor_sdk_common/system/concurrency:concurrency',
        '//thirdparty/monitor_sdk_common/text:wildcard',
        '//thirdparty/monitor_sdk_common/base/string:string',
        '//thirdparty/monitor_sdk_common/storage/file/aioframe:aioframe',
        #'//thirdparty/glog:glog',
        '//thirdparty/gflags:gflags',
    ],
    link_all_symbols = 1
)

cc_library(
    name = 'hdfs_file',
    srcs = [
        'hdfs_file.cpp',
    ],
    deps = [
        ':file2',
        '//thirdparty/monitor_sdk_common/system:system',
        '//thirdparty/monitor_sdk_common/system/concurrency:concurrency',
        '//thirdparty/monitor_sdk_common/text:wildcard',
        '//thirdparty/glog:glog',
        #'//thirdparty/libhdfs:hdfs'
        '//thirdparty/libhdfs-2.5.0:hdfs'
    ],
    incs = [ '../../libaio/src' ],
    link_all_symbols = 1
)

cc_test(
    name = 'hdfs_file_test',
    srcs = 'hdfs_file_test.cpp',
    deps = [
        ':hdfs_file',
        '//thirdparty/monitor_sdk_common/random:random',
        '//thirdparty/gmock:gmock',
    ],
    #extra_linkflags='./thirdparty/libhdfs/libjvm.so',
    extra_linkflags='./thirdparty/libhdfs-2.5.0/libjvm.so',
    warning = False,
)


"""
import os
JAVA_HOME = os.getenv('JAVA_HOME')

if JAVA_HOME:
    JAVA_INCLUDE = JAVA_HOME + '/include'
    cc_library(
        name = 'hdfs_file',
        srcs = [
            'hdfs_file.cpp',
        ],
        incs = [
            JAVA_INCLUDE,
            JAVA_INCLUDE + '/linux',
        ],
        deps = [
            ':file',
            '//thirdparty/monitor_sdk_common/base/string:string',
            '//thirdparty/monitor_sdk_common/system:system',
            '//thirdparty/monitor_sdk_common/system/concurrency:concurrency',
            '//thirdparty/monitor_sdk_common/text:wildcard',
            '//thirdparty/glog:glog',
            #'//thirdparty/libhdfs:hdfs'
            '//thirdparty/libhdfs-2.5.0:hdfs'
        ],
        link_all_symbols = 1,
    )

    cc_test(
        name = 'hdfs_file_test',
        srcs = 'hdfs_file_test.cpp',
        incs = [
            JAVA_INCLUDE,
            JAVA_INCLUDE + '/linux',
        ],
        deps = [
            ':hdfs_file',
            '//thirdparty/monitor_sdk_common/random:random',
            '//thirdparty/gmock:gmock',
        ],
        warning = False,
    )

"""

cc_library(
    name = 'file_flags',
    srcs = 'file_flags.cpp',
    deps = [
        '//thirdparty/gflags:gflags'
    ]
)

cc_library(
    name = 'file_path',
    srcs = 'file_path.cpp',
    deps = [
        ':file_flags'
    ]
)

cc_library(
    name = 'file_stream',
    srcs = [
        'file_stream.cpp'
    ],
    deps = [
        ':file_flags',
        ':file_path',
        '//thirdparty/monitor_sdk_common/base/string:string',
        '//thirdparty/glog:glog'
    ]
)

cc_library(
    name = 'hdfs_file_stream',
    srcs = [
        'hdfs_file_stream.cpp',
    ],
    deps = [
        ':file_stream',
        '//thirdparty/monitor_sdk_common/storage/file/hdfs:hdfs_client',
    ],
)

cc_test(
    name = 'file_stream_test',
    srcs = 'file_stream_test.cpp',
    deps = [
        ':file_stream'
    ],
    testdata = [
        './testdata/testdata.dat'
    ]
)

cc_test(
    name = 'hdfs_file_stream_test',
    srcs = 'hdfs_file_stream_test.cpp',
    deps = ':hdfs_file_stream',
    #extra_linkflags=['./thirdparty/libhdfs/libjvm.so'],
    extra_linkflags=['./thirdparty/libhdfs-2.5.0/libjvm.so'],
    incs = [ '../../../libaio/src' ],
)

cc_library(
    name = 'file_mock2',
    srcs = 'file_mock2.cpp',
    deps = [
        ':file2',
        '//thirdparty/gmock:gmock'
    ]
)

cc_library(
    name = 'file_mock2_class',
    srcs = 'file_mock2_class.cpp',
    deps = ':file_mock2',
    link_all_symbols = True
)

cc_library(
    name = 'file_mock',
    srcs = 'file_mock.cpp',
    deps = [
        ':file_mock2',
        ':file_mock2_class',
        ':local_file2',
    ]
)
