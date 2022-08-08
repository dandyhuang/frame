cc_library(
    name = 'keyword_shield',
    srcs = [
        'KeywordShield.cpp',
        'ACNode.cpp',
    ],
    #deps = '//thirdparty/gflags:gflags',
)

cc_library(
    name = 'hanzi_pinyin',
    srcs = [
        'ChinesePinYin.cpp',
        'PinYinMap.cpp',
    ],
    #deps = '//thirdparty/gflags:gflags',
)

cc_test(
    name = 'ac_test',
    srcs = 'ac_test.cpp',
    deps = ':keyword_shield'
)

#cc_test(
#    name = 'hanzi_pinyin_test',
#    srcs = 'hanzi_pinyin_test.cpp',
#    deps = ':hanzi_pinyin'
#)

