//
// Created by 吴婷 on 2020-06-16.
//
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <chrono>
#include <utility>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "thirdparty/monitor_sdk_common/base/string/string_piece.h"
#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/net/uri/uri.h"
#include "utils/utils.h"
#include "constants.h"
#include "thirdparty/monitor_sdk_common_params.h"

using namespace common;

long VnsTimeUtils::GetCurrentTimeMills() {
    using namespace std::chrono;
    milliseconds ms = duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
    );
    return long(ms.count());
}

long VnsTimeUtils::GetCurrentTimeMicros() {
    using namespace std::chrono;
    microseconds ms = duration_cast<microseconds>(
            system_clock::now().time_since_epoch()
    );
    return long(ms.count());
}

long VnsFileUtils::GetFileLastModifyTime(const std::string &full_filename){
    // 1.判断目录是否存在
    if(access(full_filename.c_str(), 0) == -1){
        return -1;  // 目录不存在直接返回-1
    }
    // 2.获取时间信息
    struct stat buf;
    FILE *pFile = fopen(full_filename.data(), "r");
    int fd = fileno(pFile);
    fstat(fd, &buf);
    long time = buf.st_mtime;
    fclose(pFile);
    return time;
}

bool VnsFileUtils::AccessDirExist(const std::string &dir){
    if(access(dir.c_str(), 0) == 0){
        // 目录存在，直接返回
        return true;
    }
    // 目录不存在
    std::vector<std::string> create_dirs;   // 待创建的目录列表
    create_dirs.push_back(dir);             // 首次添加

    std::string last_dir = dir;             // 外层目录
    // 取上一层目录
    std::string cur_dir = RemoveLastPathDir(last_dir);
    while (cur_dir != last_dir) {
        if(access(cur_dir.c_str(), 0) == 0){    // 若当前目录存在，跳出
            break;
        }
        // 往待创建目录列表添加
        create_dirs.push_back(cur_dir);

        last_dir = cur_dir;
        cur_dir = RemoveLastPathDir(last_dir);
    }

    int len = create_dirs.size();
    //std::cout << "目录列表大小：" << len << std::endl;
    // 依次创建目录
    for(int i=len-1; i>=0; i--){
        //std::cout << "创建目录：" << create_dirs[i] << std::endl;
        int flag = mkdir(create_dirs[i].c_str(), 0777);    // linux: 创建目录
        if(flag != 0){
            return false;   // 创建成功，返回false
        }
    }

    return true;
}

std::string VnsFileUtils::RemoveLastPathDir(const std::string &dir){
    if(dir.find('/') == StringPiece::npos){
        return dir;
    }
    // 从右往左
    int index = dir.rfind('/');
    return dir.substr(0, index);
}

std::string NetUtils::localIP(){
    // 获取一次之后，直接返回
    if(LocalProperties::ip != ""){
        return LocalProperties::ip;
    }

    FILE *pp = popen("ip a | grep -Ev 'lo:|127.0.0.1|tunl'|awk -F '[ /]+' '/inet / {print $3}'|sort -t. -k 1,3 -k 4.1,4.3 -n | head -1", "r");
    if(NULL == pp){
        return "0.0.0.0";
    }

    char ip[16] = {0};
    fgets(ip, sizeof(ip), pp);
    pclose(pp);

    if(strlen(ip) <= 1){
        return "0.0.0.0";
    }

    LocalProperties::ip = StringTrim(std::string(ip));

    return LocalProperties::ip;
}

std::string VnsStrUtils::BoolToString(bool val){
    return val ? "true" : "false";
}

/**
 * 将参数值转换为k-v字符串，比如key1=val1&key2=val2&key3=val3
 * @param param_map
 * @return
 */
std::string VnsStrUtils::mapToKeyValueStr(const std::map<std::string, std::string> &param_map) {
    if(param_map.empty() || param_map.size() == 0){
        return "";
    }
    std::string content;
    for (auto iter = param_map.begin(); iter != param_map.end(); ++iter) {
        std::string keyValue = iter->first +  "=" + URI::EncodeComponent(StringTrim(iter->second)); //等号连接
        //第一个前面不连&
        if(iter == param_map.begin()){
            content = content + keyValue;
        } else {
            content = content + "&" + keyValue;
        }
    }
    return content;
}

std::string NamingUtils::getGroupedName(const std::string &service_name, const std::string &group_name){
    return group_name + Constants::SERVICE_INFO_SPLITER + service_name;
}