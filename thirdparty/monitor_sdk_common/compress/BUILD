cc_library(
    name = 'compress',
    srcs = [
        'bmz/bmdiff.cpp',
        'bmz/bmz.cpp',
        'lzo/minilzo.c',
        'quicklz/quicklz.cpp',
        'block_compression_codec_bmz.cpp',
        'block_compression_codec.cpp',
        'block_compression_codec_lzo.cpp',
        'block_compression_codec_manager.cpp',
        'block_compression_codec_none.cpp',
        'block_compression_codec_quicklz.cpp',
        'block_compression_codec_snappy.cpp',
        'block_compression_codec_zlib.cpp',
        'block_compression_codec_gzip.cpp',
    ],
    deps = [
        '//thirdparty/snappy-1.0.2:snappy',
        '//thirdparty/monitor_sdk_common/system/concurrency:concurrency',
        '#z',
        './checksum:checksum'
    ]
)

cc_test(
    name = 'block_compression_codec_test',
    srcs = 'block_compression_codec_test.cpp',
    deps = ':compress',
    testdata = 'data'
)

