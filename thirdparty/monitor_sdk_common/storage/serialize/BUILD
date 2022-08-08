# Copyright 2012 Yunrang Inc. All Rights Reserved.
# Author: pittzhao413@163.com(ZhaoPu/11091180)
#
# Description:

cc_library(name = 'serio',
           srcs = ['serio.cpp',
                   ],
           deps = [
                   ],
           )

cc_binary(
    name = 'serio_test',
    srcs = 'serio_test.cpp',
	deps = [':serio'],
    extra_cppflags = [ 
        '-std=c++11',
        '-Wall',
        '-Werror',
        '-O1',
    ], 
)