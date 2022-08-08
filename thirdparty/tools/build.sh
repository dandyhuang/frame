 if [ ! -d "blade-build" ];then
     git clone https://gitlab.vmic.xyz/11121440/blade-build.git
 fi
 source thirdparty/tools/source.sh
 #git clone https://gitlab.vmic.xyz/iai_content/zeus_conf.git
 cd blade-build && git checkout . && git pull  && cd -
 openssl=$(which openssl)
 if [ -z $openssl ]; then
    yum install openssl-devel  -y
 fi
 source thirdparty/tools/source.sh
 git submodule update --init --recursive
 ./blade-build/blade build
sh thirdparty/tools/deploy_build_lib.sh
