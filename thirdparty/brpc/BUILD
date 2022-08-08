cc_library(
    name='brpc',
    prebuilt=True,
    export_incs=['./include',],
    deps = [
        ":butil",
        ":bthread",
        ":bvar",
        ":json2pb",
        ":mcpack2pb",
        ":cc_brpc_internal_proto",
	":cc_brpc_idl_options_proto",
        "//thirdparty/protobuf:protobuf",
        "//thirdparty/protobuf:protoc",
        ],
)

cc_library(
    name='cc_brpc_idl_options_proto',
    prebuilt=True,
    export_incs=['./include',],
    deps = ['//thirdparty/leveldb:leveldb',],
)

cc_library(
    name='butil',
    prebuilt=True,
    export_incs=['./include',],
    deps = ['//thirdparty/leveldb:leveldb',],
)
cc_library(
    name='bthread',
    prebuilt=True,
    export_incs=['./include',],
    deps = ['//thirdparty/brpc:butil',
	   '//thirdparty/leveldb:leveldb',],
)
cc_library(
    name='bvar',
    prebuilt=True,
    export_incs=['./include',],
    deps = ['//thirdparty/brpc:butil',
	    '//thirdparty/leveldb:leveldb',],
)
cc_library(
    name='json2pb',
    prebuilt=True,
    export_incs=['./include',],
    deps = ['//thirdparty/brpc:butil',
            '//thirdparty/leveldb:leveldb',],
)
cc_library(
    name='mcpack2pb',
    prebuilt=True,
    export_incs=['./include',],
    deps = ['//thirdparty/leveldb:leveldb',],
)
cc_library(
    name='cc_brpc_internal_proto',
    prebuilt=True,
    export_incs=['./include',],
    deps = ['//thirdparty/leveldb:leveldb',],
)
