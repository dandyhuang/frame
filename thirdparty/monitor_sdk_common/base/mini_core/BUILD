cc_library(name = 'mini_core',
           srcs =
               [
                 'vma.c',
                 'bthelper.c',
                 'mem.c',
                 'exception.c',
               ],
           deps =
               [
                 '//thirdparty/libunwind:unwind64',
               ],
           extra_cppflags =
               [
                 '-static',
               ], )

cc_test(name = 'multi_thread_test', srcs = 'multi_thread_test.c', deps = [
      ':mini_core',
      '#pthread',
    ])
