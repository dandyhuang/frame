//
// Created by 吴婷 on 2020-05-14.
//

#ifndef SSO_CODE_SSO_CLIENT_H
#define SSO_CODE_SSO_CLIENT_H

#include <iostream>
#include <map>
#include <memory>

#include "result_vo.h"
#include "config_params.h"
#include "http_invoker.h"

namespace common {
/**
 * 单点登录客户端
 */
class SSOClient {
public:
    /**
     * 构造函数
     * @param app_code cmdb服务名
     * @param aes_key AES密钥
     * @param aes_iv AES偏移量
     */
    SSOClient(const std::string& app_code, const std::string& aes_key, const std::string& aes_iv);

    ~SSOClient(){};

    /**
     * 检查登录状态
     * @param request_url 业务请求url
     * @param request_headers 业务请求头
     * @param redirect_url 校验不过，期望跳转的页面
     * @return 返回结果
     */
    SSOResult CheckLoginStatus(const std::string& request_url, std::map<std::string, std::string> request_headers, const std::string& redirect_url);

    /**
     * 执行退出操作
     * @param request_url 业务请求url
     * @param request_headers 业务请求头
     * @param redirect_url 校验不过，期望跳转的页面
     * @return
     */
    SSOResult DoLogoutStep(const std::string& request_url, std::map<std::string, std::string> request_headers, const std::string& redirect_url);


private:
    /**
     * 构造Login或者Logout参数: token判断
     * @param request_url 业务请求url
     * @param request_headers 业务请求头
     * @param redirect_url 校验不过，期望跳转的页面
     * @param is_login 是否为登录参数login
     * @return 登录请求参数
     */
    std::string GetSignParam(const std::string& request_url, std::map<std::string, std::string> request_headers,
            const std::string& redirect_url, bool is_login);


private:
    std::string _app_code;       //cmdb服务名
    std::string _aes_key;        //AES密钥
    std::string _aes_iv;         //AES偏移量

    std::shared_ptr<SSOHttpInvoker> _http_invoker_ptr;

};

}

#endif //SSO_CODE_SSO_CLIENT_H
