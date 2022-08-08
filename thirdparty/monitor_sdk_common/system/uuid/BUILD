cc_library(
    name = "uuid",
    srcs = [
        "libuuid/clear.c",
        "libuuid/copy.c",
        "libuuid/gen_uuid_nt.c",
        "libuuid/pack.c",
        "libuuid/unparse.c",
        "libuuid/compare.c",
        "libuuid/gen_uuid.c",
        "libuuid/isnull.c",
        "libuuid/parse.c",
        "libuuid/unpack.c",
        "libuuid/uuid_time.c"
    ],
    extra_cppflags = ["-Wno-format-nonliteral", "-D_POSIX_C_SOURCE=200809L"]
)

cc_test(
    name = "uuid_test",
    srcs = "uuid_test.cpp",
    deps = ":uuid"
)

