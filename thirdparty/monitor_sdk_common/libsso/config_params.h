//
// Created by 吴婷 on 2020-05-14.
//

#ifndef COMMON_LIBSSO_CONFIG_PARAMS_H
#define COMMON_LIBSSO_CONFIG_PARAMS_H

#include <iostream>

namespace common {

/**
 * 初始化验证信息
 */
class SSOProperties {
public:
    static std::string SSODOMAIN;      //sso验证环境
};

/**
 * token类型
 */
class TokenType {
public:
    static std::string TOKEN;   //token
    static std::string MTOKEN;  //mtoken
};

/**
 * token位置
 */
class TokenLocation {
public:
    static std::string URL;
    static std::string COOKIE;
    static std::string UNKNOWN;
};

/**
 * 返回结果描述
 */
class ResultMsg {
public:
    static std::string SUCCESS;     //0：成功
    static std::string PARAMFAIL;   //-1: 参数校验失败
    static std::string REDIRECT;    //-9999: 需要重定向
};

/**
 * 用户请求
 */
class UserRequest {
public:
    UserRequest();
    UserRequest(const std::string &url, const std::string &headers) : url(url), headers(headers){};
    ~UserRequest();

public:
    std::string url;                // 请求url
    std::string headers;            // 请求头信息
    //std::string params;             // 请求参数
};

/**
 * 登录状态参数
 */
class LoginParam {
public:
    std::string ToString();     //转换为jsonstr

public:
    std::string location;       // 取值: url, cookie, unknown
    std::string url;            // 校验不过，期望跳转的页面
    std::string type;           // 取值: token, mtoken
    std::string token;          // token/mtoken的取值
    std::string ip;             // 本机ip
    std::string app_code;
    std::string date_time;
};


/**
 * 单点登录 api输入参数
 */
class SSORequest {
public:
    SSORequest(){};
    SSORequest(const std::string &sign, const std::string &app_code, const std::string &date_time)
    : sign(sign)
    , app_code(app_code)
    , date_time(date_time){};

    ~SSORequest(){};

    std::string ToString(); //转换为jsonstr

public:
    std::string sign;       // 业务参数AES加密后的字符串
    std::string app_code;   // cmdb服务名
    std::string date_time;  // 发起时间戳
};

/**
 * 单点登录 api响应参数
 */
class SSOResponse {
public:
    int code;           // 0-成功；-1：参数校验失败；-9999：页面需要重定向
    std::string msg;    // 错误描述
    std::string data;   // 当前登录人
    std::string redirect_url;   // 重定向页面
};

}

#endif //COMMON_LIBSSO_CONFIG_PARAMS_H
