//
// Created by 吴婷 on 2020-05-15.
//

#ifndef COMMON_LIBSSO_UTILS_H
#define COMMON_LIBSSO_UTILS_H

#include <iostream>
#include <memory>
#include <map>

#include "config_params.h"

namespace common {

class SSOTimeUtils{
public:
    static long GetCurrentTimeMills();      //毫秒级
    static long GetCurrentTimeMicros();     //微秒级
};

/**
 * api内容反序列化填充到各个类
 */
class SSOParseUtils {
public:
    static bool ParseSSOResponse(const std::string& json_str, std::shared_ptr<SSOResponse> ret);   //解析填充SSO返回类
    static std::map<std::string, std::string> ParseMap(const std::string& json_str);              //将json数组解析为map
};

class HttpUtils {
public:
    // 根据请求头获取ip
    static std::string GetIpAddress(std::map<std::string, std::string> request_headers);
};

}   // namespace common

#endif //COMMON_LIBSSO_UTILS_H
