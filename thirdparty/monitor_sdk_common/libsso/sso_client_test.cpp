//
// Created by 吴婷 on 2020-05-14.
//
#include <memory>
#include <iostream>

#include "sso_client.h"

using namespace common;

int main(){

    // client初始化
    std::string app_code("privilege-web");       //cmdb服务名
    std::string aes_key("vtrqMI5mR5urODuG");        //AES密钥
    std::string aes_iv("9686128666182599");         //AES偏移量
    std::shared_ptr<SSOClient> client = std::make_shared<SSOClient>(app_code, aes_key, aes_iv);

    // 1.登录校验
    // 请求url
    std::string request_url = "http://km.vivo.xyz/pages/viewpage.action?pageId=128195702";
    //std::string request_url = "http://km.vivo.xyz/pages/viewpage.action?token=vF5r_KvGXkyqMMK4TB6.kAWnO8vqJL9r7GVqcKYKvsrYg9L9uDZI0G6BYfksxar4bsbxHRe8Hbc*";

    // 请求headers
    std::map<std::string, std::string> request_headers;
    request_headers["X-Forwarded-For"] = "10.13.184.104";    //ip与token一一对应
    request_headers["Cookie"] = "uuc-token=111; token=vF5r_KvGXkyqMMK4TB6.kOXTWpJL_xdRE8ZF7oYLfETWF79WLR_m0YENYzZily7aM6pD5rz6jcI*;";
    // 验证不过，期望跳转的url
    std::string redirect_url = "https://web-tars.vmic.xyz/";

    SSOResult result = client->CheckLoginStatus(request_url, request_headers, redirect_url);
    std::cout << "登录校验结果：" << result.ToString() << std::endl;

    // 2.退出校验
    std::string request_url2 = "http://km.vivo.xyz/pages/viewpage.action?token=vF5r_KvGXkyqMMK4TB6.kOXTWpJL_xdRE8ZF7oYLfETWF79WLR_m0YENYzZily7aM6pD5rz6jcI*";
    // 请求headers
    std::map<std::string, std::string> request_headers2;
    result = client->DoLogoutStep(request_url2, request_headers2, redirect_url);

    std::cout << "\n退出校验结果："  << result.ToString() << std::endl;


    return 0;
}

