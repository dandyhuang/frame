cc_library(
    name = 'hdfs_client',
    srcs = [
        'hdfs_client.cpp',
        'hdfs_def.cpp',
        'hdfs_file.cpp',
    ],
    deps=[
        '//thirdparty/monitor_sdk_common/system/concurrency:this_thread',
        '//thirdparty/monitor_sdk_common/system/concurrency:sync_object',
        '//thirdparty/glog:glog',
        '//thirdparty/gflags:gflags',
       '//thirdparty/gmock:gmock',
        #'//thirdparty/libhdfs:hdfs',
        '//thirdparty/libhdfs-2.5.0:hdfs',
    ],
)

'''
cc_test(
    name = 'hdfs_test',
    srcs = 'hdfs_test.cpp',
    deps = [
        ':hdfs_client',
    ],
    testdata = [
        './testdata/data.txt',
    ],
    extra_linkflags=['./thirdparty/libhdfs/libjvm.so'],
    heap_check='strict'
)
'''

