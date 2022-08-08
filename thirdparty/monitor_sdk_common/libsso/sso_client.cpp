//
// Created by 吴婷 on 2020-05-14.
//
#include <vector>

#include "utils.h"
#include "sso_client.h"
#include "crypto_aes.h"

#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/net/uri/query_params.h"

using namespace common;

const std::string LOGIN_ADDRESS = "/dnssNoCheck/api/access/checkLogin";
const std::string LOGOUT_ADDRESS = "/dnssNoCheck/api/access/doLogout";

SSOClient::SSOClient(const std::string& app_code, const std::string& aes_key, const std::string& aes_iv)
: _app_code(app_code), _aes_key(aes_key), _aes_iv(aes_iv) {
    _http_invoker_ptr = std::make_shared<SSOHttpInvoker>();
};

SSOResult SSOClient::CheckLoginStatus(const std::string& request_url, std::map<std::string, std::string> request_headers, const std::string& redirect_url){
    SSOResult result;
    // 登录验证接口
    std::string url = SSOProperties::SSODOMAIN + LOGIN_ADDRESS;
    // 登录请求参数
    std::string payload = GetSignParam(request_url, request_headers, redirect_url, true);
    //std::cout << "登录请求参数：" << payload << std::endl;
    std::shared_ptr<SSOHttpInvoker> http_invoker_ptr = std::make_shared<SSOHttpInvoker>();
    std::string ret = http_invoker_ptr->doHttpPost(url, payload);
    // SSOResponse
    std::shared_ptr<SSOResponse> response = std::make_shared<SSOResponse>();
    if(!SSOParseUtils::ParseSSOResponse(ret, response)) {
        // 解析返回结果识别
        return result;
    }
    // 填充返回结果
    if(response->code == 0){
        result.success = true;
        result.code = 0;
        result.msg = ResultMsg::SUCCESS;
    } else if(response->code == -1) {
        result.success = false;
        result.code = -1;
        result.msg = ResultMsg::PARAMFAIL;
    } else if(response->code == -9999) {
        result.success = false;
        result.code = -9999;
        result.msg = ResultMsg::REDIRECT;
        result.redirect_url = response->redirect_url;
    }

    return result;
}

SSOResult SSOClient::DoLogoutStep(const std::string& request_url, std::map<std::string, std::string> request_headers, const std::string& redirect_url){
    SSOResult result;
    // 退出验证接口
    std::string url = SSOProperties::SSODOMAIN + LOGOUT_ADDRESS;
    // 退出请求参数
    std::string payload = GetSignParam(request_url, request_headers, redirect_url, false);
    //std::cout << "退出请求参数：" << payload << std::endl;
    std::shared_ptr<SSOHttpInvoker> http_invoker_ptr = std::make_shared<SSOHttpInvoker>();
    std::string ret = http_invoker_ptr->doHttpPost(url, payload);
    // SSOResponse
    std::shared_ptr<SSOResponse> response = std::make_shared<SSOResponse>();
    if(!SSOParseUtils::ParseSSOResponse(ret, response)) {
        // 解析返回结果识别
        return result;
    }
    // 填充返回结果
    if(response->code == -9999){
        result.success = true;
        result.code = -9999;
        result.msg = ResultMsg::REDIRECT;
        result.redirect_url = response->redirect_url;  //退出成功，有重定向页面
    } else if(response->code == -1){
        result.success = false;
        result.code = -1;
        result.msg = ResultMsg::PARAMFAIL;
    }

    return result;
}

std::string SSOClient::GetSignParam(const std::string& request_url, std::map<std::string, std::string> request_headers, const std::string& redirect_url, bool is_login){
    // 1.登录参数
    bool is_ready = false;   //token是否已经确定
    LoginParam login_param;
    login_param.app_code = _app_code;
    // 校验不过，希望跳转的网址
    login_param.url = redirect_url;

    // (1)判断请求url里面是否有token参数
    QueryParams params;
    if(params.ParseFromUrl(request_url)){ //解析url
        std::string token;
        if(params.GetValue("token", &token)){           //url取到token
            login_param.token = token;
            login_param.type = TokenType::TOKEN;
            login_param.location = TokenLocation::URL;
            is_ready = true;
        } else if(params.GetValue("mtoken", &token)){   //url取到mtoken
            login_param.token = token;
            login_param.type = TokenType::MTOKEN;
            login_param.location = TokenLocation::URL;
            is_ready = true;
        }
    }
    // (2)判断cookie是否含token信息
    if(!is_ready){ //token不在url中
        auto iter = request_headers.find("Cookie");
        if(iter != request_headers.end()){  //找到Cookie
            const std::string & cookie = iter->second;
            // 先整体cookie字符串判断token是否存在，如存在按;进行分隔，然后逐项判断是否为token项，取出token具体内容
            if(cookie.find("token") != cookie.npos){   //cookie含token
                // 按;进行分割
                std::vector<std::string> vec;
                SplitString(cookie, ";", &vec);
                for (unsigned int i = 0; i < vec.size(); ++i) {
                    std::string kv = vec[i];
                    if(kv.find("token") != kv.npos){    //单项含token
                        //按=进行分割
                        int index = kv.find('=');
                        std::string key = StringTrim(kv.substr(0, index));  //去除空格
                        //确定token类型：token, mtoken, 其他token
                        if("token" == key){
                            login_param.token = StringTrim(kv.substr(index+1, kv.length())); //token值
                            login_param.location = TokenLocation::COOKIE;
                            login_param.type = TokenType::TOKEN;
                            is_ready = true;
                            break;
                        }
                        if("mtoken" == key){
                            login_param.token = StringTrim(kv.substr(index+1, kv.length())); //token值
                            login_param.location = TokenLocation::COOKIE;
                            login_param.type = TokenType::MTOKEN;
                            is_ready = true;
                            break;
                        }
                    }
                }
            }
        }
    }
    // (3)没有token信息
    if(!is_ready){
        login_param.location = TokenLocation::UNKNOWN;
    }
    // (4)登录和退出[无ip和location]参数区分
    if (is_login) {
        // 本机ip
        login_param.ip = HttpUtils::GetIpAddress(request_headers);
    } else {
        login_param.location = "";
    }

    // 当前时间戳
    std::string date_time = IntegerToString(SSOTimeUtils::GetCurrentTimeMills());
    login_param.date_time = date_time;
    //std::cout << "\nsign参数：" << login_param.ToString() << std::endl;

    // 2.AES加密
    std::string sign = CryptoAes::GetEncodedSign(login_param.ToString(), _aes_key, _aes_iv);

    // 3.单点登录 api输入参数
    SSORequest request;
    request.sign = sign;
    request.app_code = _app_code;
    request.date_time = date_time;

    return request.ToString();
}
