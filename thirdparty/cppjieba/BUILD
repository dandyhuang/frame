cc_library(
    name='jieba',
    export_incs=['./include', './deps']
)
cc_binary(
    name="test",
    srcs= ["./test/demo.cpp"],
    deps = ["//thirdparty/cppjieba:jieba"]
)
