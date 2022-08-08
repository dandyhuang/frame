cc_library(
    name='boost',
    export_incs=['./include'],
)
cc_library(
    name='boost_filesystem',
    prebuilt=True,
    export_incs=['./include',]
)

cc_library(
    name='boost_system',
    prebuilt=True,
    export_incs=['./include',]
)

cc_library(
    name='boost_locale',
    prebuilt=True,
    export_incs=['./include',],
    deps = ['//thirdparty/boost:boost_system',]
)

cc_library(
    name='boost_thread',
    prebuilt=True,
    export_incs=['./include',],
)

cc_library(
    name='boost_regex',
    prebuilt=True,
    export_incs=['./include',],
)

