cc_library(
    name = 'binary_version',
    srcs = [
        'binary_version.cpp',
        'setup_binary_version.cpp',
    ],
    deps = '//thirdparty/gflags:gflags',
)

cc_test(
    name = 'binary_version_test',
    srcs = 'binary_version_test.cpp',
    deps = [
        ':binary_version'
    ]
)

cc_library(
    name = 'static_resource',
    srcs = 'static_resource.cpp',
    deps = '//thirdparty/monitor_sdk_common/base/string:string'
)

resource_library(
    name = 'static_resource_test_data',
    srcs = 'static_resource.testdata',
)

cc_library(name = 'byte_order', srcs = 'byte_order.cpp')

cc_library(
    name = 'class_registry',
    deps = './class_registry:_class_registry',
)

cc_library(
    name = 'mini_core',
    deps = '//thirdparty/monitor_sdk_common/base/mini_core:mini_core',
)

cc_library(
    name = 'closure',
    srcs = 'closure.cpp',
)

cc_library(
    name = 'benchmark',
    srcs = 'benchmark.cpp',
    deps = [
        '//thirdparty/monitor_sdk_common/system/time:time',
        '//thirdparty/glog:glog',
        '//thirdparty/perftools:tcmalloc_and_profiler'
    ]
)

cc_library(
    name = 'benchmark_main',
    srcs = 'benchmark_main.cpp',
    deps = [
        ':benchmark',
        '//thirdparty/gflags:gflags',
    ]
)

cc_library(
    name = 'int128',
    srcs = 'int128.cpp',
    deps = '//thirdparty/glog:glog',
)

cc_library(
    name = 'uint32_divisor',
    srcs = 'uint32_divisor.cpp'
)

cc_library(
    name = 'random',
    srcs = 'random.cpp',
)

cc_library(
    name = 'arena',
    srcs = 'arena.cpp',
)

cc_library(
    name = 'shmutil',
    srcs = 'shm_util.c',
)

cc_binary(
    name = 'tocken_bucket_test',
    srcs = 'tocken_bucket_test.cpp',
    deps = [
        '//thirdparty/monitor_sdk_common/system/event_dispatcher:event_dispatcher',
    ],
)

cc_test(
    name = 'uint32_divisor_test',
    srcs = 'uint32_divisor_test.cpp',
    deps = [
        ':uint32_divisor',
        '//thirdparty/gtest:gtest',
    ]
)

cc_test(
    name = 'arena_test',
    srcs = 'arena_test.cpp',
    deps = [
        ':arena',
        ':random',
    ]
)

cc_test(
    name = 'static_resource_test',
    srcs = 'static_resource_test.cpp',
    deps = [
        ':static_resource',
        ':static_resource_test_data',
    ],
)

cc_test(name = 'array_size_test', srcs = 'array_size_test.cpp')
cc_test(name = 'byte_order_test', srcs = 'byte_order_test.cpp')
cc_test(name = 'closure_test', srcs = 'closure_test.cpp', deps = ':closure')
cc_test(name = 'closure_test2', srcs = 'closure_test2.cpp')       
cc_test(name = 'functional_test', srcs = 'functional_test.cpp')
cc_test(name = 'intrusive_list_test', srcs = 'intrusive_list_test.cpp')
cc_test(name = 'preprocess_test', srcs = 'preprocess_test.cpp')
cc_test(name = 'shared_ptr_test', srcs = 'shared_ptr_test.cpp')
cc_test(name = 'singleton_test', srcs = 'singleton_test.cpp')
cc_test(name = 'static_assert_test', srcs = 'static_assert_test.cpp')
cc_test(name = 'stl_util_test', srcs = 'stl_util_test.cpp')
cc_test(name = 'unique_ptr_test', srcs = 'unique_ptr_test.cpp')
cc_test(name = 'unordered_test', srcs = 'unordered_test.cpp')
cc_test(name = 'stdint_test', srcs = 'stdint_test.cpp')


cc_library(
    name = 'export_variable',
    srcs = 'export_variable.cpp',
    deps = [
        '//thirdparty/monitor_sdk_common/system/concurrency:concurrency',
        '//thirdparty/jsoncpp:jsoncpp'
    ]
)

cc_test(
    name = 'export_variable_test',
    srcs = 'export_variable_test.cpp',
    deps = [
        ':export_variable'
    ]
)

cc_library(
    name = 'module',
    srcs = [
        'module/module.cpp',
        'module/dependency_map.cpp'
    ],
    deps = [
	'//thirdparty/monitor_sdk_common/system/concurrency:concurrency',
        '//thirdparty/gtest:gtest',
        '//thirdparty/glog:glog',
        ':binary_version'
    ]
)

cc_library(
    name = 'barrier_closure',
    srcs = 'barrier_closure.cpp'
)

cc_test(
    name = 'barrier_closure_test',
    srcs = 'barrier_closure_test.cpp',
    deps = ':barrier_closure'
)

cc_library(
    name = 'counter',
    srcs = 'counter.cpp',
    deps = [
        '//thirdparty/glog:glog',
        '//thirdparty/monitor_sdk_common/system:check_error'
    ]
)

cc_test(
    name = 'counter_test',
    srcs = 'counter_test.cpp',
    deps = [
        ':counter',
        '//thirdparty/monitor_sdk_common/system/concurrency:this_thread'
    ],
    heap_check = 'strict'
)

cc_test(
    name = 'global_initialize_test',
    srcs = 'global_initialize_test.cpp'
)

cc_test(
    name = 'strict_bool_test',
    srcs = 'strict_bool_test.cpp',
    deps = [':ref_counted', '//thirdparty/glog:glog']
)

cc_library(
    name = 'ref_counted',
    srcs = 'ref_counted.cpp'
)

cc_test(
    name = 'ref_counted_test',
    srcs = 'ref_counted_test.cpp',
    deps = ':ref_counted'
)

cc_library(
    name = 'function',
    srcs = [
        'function/function_base.cpp'
],
    deps = ':ref_counted'
)

cc_test(
    name = 'function_test',
    srcs = [
        'function_test.cpp'
    ],
    deps = [
        '//thirdparty/monitor_sdk_common/base:function',
        '//thirdparty/gmock:gmock'
    ]
)

cc_test(name = 'container_of_test', srcs = 'container_of_test.cpp')

cc_test(
    name = 'any_ptr_test',
    srcs = 'any_ptr_test.cpp',
)

cc_test(
    name = 'annotation_test',
    srcs = 'annotation_test.cpp'
)

