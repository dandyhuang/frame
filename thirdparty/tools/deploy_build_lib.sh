#!/bin/sh

# get lib depend
function CopyLib () {
  file="$@"
  echo "get lib for $file "
  #ldd
  libs=`ldd  $file |awk '{print $1}' |grep -v '/' `
  for line in $libs
  do
    echo $line
    path_lib=$( find ./deploy/lib/  -name $line )
    if [ "$path_lib"  ]
    then
      continue
    fi
    path=$( find ./thirdparty/  -name $line )
    if [ "$path" ]
    then
      echo "cp $path ./deploy/lib/"
      cp $path ./deploy/lib/
    fi
  done
  #strings lib.*so
  libs=`strings  $file |grep   '^lib.*so' |grep -v '/'`
  for line in $libs
  do
    path_lib=$( find ./deploy/lib/  -name $line )
    if [ "$path_lib"  ]
    then
      continue
    fi
    path=$( find ./thirdparty/  -name $line )
    if [ "$path"  ]
    then
      echo "cp $path ./deploy/lib/"
      cp $path ./deploy/lib/
    fi
  done
}



function Libchange(){
  path=$1
  lib=$2
  bin=$3
  echo $path  $lib  $bin
  patchelf --replace-needed  ${path}  ${lib} ${bin}
}



rm  -rf  ./deploy
mkdir -p  ./deploy

#produce bin
mkdir -p ./deploy/bin
cp ./blade-bin/toucan ./deploy/bin

libs=$(ldd ./blade-bin/toucan|grep build64_release|awk '{print $1}')
for line in $libs
do
   lib=$(echo $line|awk -F'/' '{print $NF}')
   Libchange $line  $lib  "./deploy/bin/toucan"
done


#produce lib for bin
mkdir deploy/lib/
CopyLib "./deploy/bin/toucan"
libs=`ldd  ./deploy/bin/toucan`
for line in $libs
do
   # if file exists
  if [ -f "./deploy/lib/$line" ];then
    echo "./deploy/lib/$line find thirdparty libs"
    CopyLib "./deploy/lib/$line"
  fi
done
cp ./thirdparty/mkl/lib64/*   ./deploy/lib/
export LD_LIBRARY_PATH=./deploy/lib:$LD_LIBRARY_PATH


#produce start

mkdir ./deploy/tools
cp  ./thirdparty/tools/*         ./deploy/tools



#zip deploy
cd  deploy
zip -r ../toucan.zip   ./*
cd ..

