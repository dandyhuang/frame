cc_library(
    name = 'this_thread',
    srcs = 'this_thread.cpp',
    deps = '#pthread'
)

cc_library(
    name = 'sync_object',
    srcs = [
        'condition_variable.cpp',
        'mutex.cpp',
        'sequence_lock.cpp',
        'spinlock.cpp',
        'event.cpp',
    ],
    deps = [
        ':this_thread',
        '//thirdparty/monitor_sdk_common/system/time:posix_time',
        '//thirdparty/monitor_sdk_common/system:check_error',
        '//thirdparty/monitor_sdk_common/system:info',
        '#pthread',
    ]
)

cc_library(
    name = 'concurrency',
    srcs = [
        'base_thread.cpp',
        'fixed_thread_pool.cpp',
        'named_pipe_unix.cpp',
        'thread.cpp',
        'thread_pool.cpp',
        'thread_types.cpp',
        'thread_group.cpp'
    ],
    deps = [
        ':this_thread',
        ':sync_object',
        '//thirdparty/monitor_sdk_common/base/compatible:compatible',
        '//thirdparty/monitor_sdk_common/base:function',
        '//thirdparty/monitor_sdk_common/system/time:time',
        '//thirdparty/monitor_sdk_common/system:system',
        '//thirdparty/monitor_sdk_common/system:check_error',
        '#pthread'
    ]
)

cc_test(
    name = 'condition_variable_test',
    srcs = 'condition_variable_test.cpp',
    deps = [
        ':concurrency'
    ]
)

cc_test(
    name = 'this_thread_test',
    srcs = 'this_thread_test.cpp',
    deps = [
        ':concurrency'
    ],
    heap_check = 'strict'
)

cc_test(
    name = 'thread_test',
    srcs = 'thread_test.cpp',
    deps = [
        ':concurrency'
    ],
    heap_check = 'strict'
)

cc_test(
    name = 'thread_pool_test',
    srcs = 'thread_pool_test.cpp',
    deps = [
        ':concurrency',
        '//thirdparty/perftools:profiler'
    ]
)

cc_test(
    name = 'fixed_thread_pool_test',
    srcs = 'fixed_thread_pool_test.cpp',
    deps = ':concurrency',
)

cc_test(
    name = 'thread_group_test',
    srcs = 'thread_group_test.cpp',
    deps = ':concurrency'
)

cc_test(
    name = 'mutex_test',
    srcs = 'mutex_test.cpp',
    deps = ':concurrency',
    heap_check = 'local'
)

cc_test(
    name = 'spinlock_test',
    srcs = 'spinlock_test.cpp',
    deps = ':concurrency'
)

cc_test(
    name = 'sequence_lock_test',
    srcs = 'sequence_lock_test.cpp',
    deps = [
        ':sync_object'
    ]
)

cc_test(
    name = 'rwlock_test',
    srcs = 'rwlock_test.cpp',
    deps = ':concurrency'
)

cc_test(
    name = 'event_test',
    srcs = 'event_test.cpp',
    deps = [
        ':concurrency',
        '//thirdparty/glog:glog'
    ]
)

cc_test(
    name = 'blocking_queue_test',
    srcs = 'blocking_queue_test.cpp',
    deps = ':concurrency'
)

cc_test(
    name = 'named_pipe_test',
    srcs = 'named_pipe_test.cpp',
    deps = ':concurrency'
)

