make clean
mkdir -p output/include
mkdir -p output/lib
blade clean
blade build -prelease
cd ../..
ROOT_DIR=`pwd`
cd common/librmq
cp $ROOT_DIR/build64_release/common/base/libclosure.a output/lib
cp $ROOT_DIR/build64_release/common/base/class_registry/lib_class_registry.a output/lib
cp $ROOT_DIR/build64_release/common/base/string/lib_compare.a output/lib
cp $ROOT_DIR/build64_release/common/base/string/lib_concat.a output/lib
cp $ROOT_DIR/build64_release/common/base/string/lib_string_piece.a output/lib
cp $ROOT_DIR/build64_release/common/base/string/lib_number.a output/lib
cp $ROOT_DIR/build64_release/common/crypto/uuid/libuuid.a output/lib
cp $ROOT_DIR/build64_release/common/base/string/format/lib_format.a output/lib
cp $ROOT_DIR/build64_release/common/base/string/lib_algorithm.a output/lib
cp $ROOT_DIR/build64_release/common/encoding/libencoding.a output/lib
cp $ROOT_DIR/build64_release/common/storage/file/libfile.a output/lib
cp $ROOT_DIR/build64_release/common/system/threading/lib_thread_group.a output/lib
cp $ROOT_DIR/build64_release/common/system/threading/lib_sync_object.a output/lib
cp $ROOT_DIR/build64_release/common/system/threading/lib_thread_pool.a output/lib
cp $ROOT_DIR/build64_release/common/system/time/libtime.a output/lib
cp $ROOT_DIR/build64_release/common/system/threading/lib_thread.a output/lib
cp $ROOT_DIR/build64_release/common/system/threading/lib_this_thread.a output/lib
cp $ROOT_DIR/build64_release/common/system/info/libinfo.a output/lib
cp $ROOT_DIR/build64_release/common/system/libcheck_error.a output/lib
cp $ROOT_DIR/build64_release/common/net/http/libtypes.a output/lib
cp $ROOT_DIR/build64_release/common/net/http/libclient.a output/lib
cp $ROOT_DIR/build64_release/thirdparty/stringencoders-svn-355/libstringencoders.a output/lib
cp $ROOT_DIR/build64_release/common/system/net/libip_address.a output/lib
cp $ROOT_DIR/build64_release/common/net/uri/liburl.a output/lib
cp $ROOT_DIR/build64_release/common/system/net/libsocket.a output/lib
cp $ROOT_DIR/build64_release/common/system/net/libdomain_resolver.a output/lib
cp $ROOT_DIR/build64_release/common/text/libwildcard.a output/lib
cp $ROOT_DIR/build64_release/common/system/net/libsocket_address.a output/lib
cp $ROOT_DIR/build64_release/common/storage/path/libpath.a output/lib
cp $ROOT_DIR/build64_release/thirdparty/libev-4.15/libev.a output/lib
cp $ROOT_DIR/build64_release/thirdparty/glog-0.3.5/src/libglog.a output/lib
cp $ROOT_DIR/build64_release/thirdparty/gflags-2.2.1/src/libgflags.a output/lib
cp $ROOT_DIR/build64_release/thirdparty/jsoncpp-1.8.4/src/lib_json/libjsoncpp.a output/lib
cp $ROOT_DIR/build64_release/thirdparty/AMQP-CPP-4.0.1/libamqp.a output/lib
cp *.h output/include
make
tar czvf librmq.tgz output
