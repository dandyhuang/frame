#!/bin/bash
PROTOBUF=../thirdparty/protobuf

protoc="${PROTOBUF}/bin/protoc"

cpp_out_path="./"
currentDir=$(dirname $0)
currentDir=$(cd ${currentDir} && pwd)

LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${PROTOBUF}/lib64
echo "[${LD_LIBRARY_PATH}]"
export LD_LIBRARY_PATH

rm -f *.h *.cc

${protoc} --cpp_out=${cpp_out_path} ./*.proto



