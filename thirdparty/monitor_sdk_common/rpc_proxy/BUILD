cc_binary(
  name = 'test_pb_loader',
  srcs = [
    'test_pb_loader.cpp',
  ],
  incs = [],
  deps = [
    ':pb_loader',
  ],

  extra_cppflags = [
    '-std=c++11',
    '-Wall',
    '-Werror',
    '-Wno-deprecated',
  ],
)

cc_binary(
  name = 'rpc_batch_get',
  srcs = [
    'rpc_batch_get.cpp',
  ],
  deps = [
    ':rpc_lib',
    '//thirdparty/monitor_sdk_common/storage/file:file',
  ],
  extra_cppflags = [
    '-std=c++11',
    '-Wall',
    '-Werror',
    '-Wno-deprecated',
  ],
)

cc_binary(
  name = 'ProxyServer',
  srcs = [
    'proxy_server.cpp',
    'proxy_impl.cpp',
  ],
  deps = [
    ':rpc_lib',
    ':proxy_proto',
    '#pthread',
  ],
  extra_cppflags = [
    '-std=c++11',
    '-Wall',
    '-Werror',
    '-Wno-deprecated',
  ],
)

cc_library(
  name = 'pb_loader',
  srcs = [
    'pb_loader.cpp',
  ],
  incs = [],
  deps = [
    '//thirdparty/monitor_sdk_common/pb_json_converter:pb-json-converter',
    '#protobuf',
    '#pthread',
  ],

  extra_cppflags = [
    '-std=c++11',
    '-Wall',
    '-Werror',
    '-Wno-deprecated',
  ],
)

cc_library(
  name = 'rpc_lib',
  srcs = [
    'rpc_lib.cpp',
  ],
  incs = [
  ],
  deps = [
    ':pb_loader',
    '//thirdparty/tars:tars',
    '//thirdparty/monitor_sdk_common/base/string:string',
  ],

  extra_cppflags = [
    '-std=c++11',
    '-Wall',
    '-Werror',
    '-Wno-deprecated',
  ],
)

proto_library(
  name = 'proxy_proto',
  srcs = ['proxy.proto'],
  plugins = ['custom']
)

