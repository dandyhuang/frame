//
// Created by 吴婷 on 2020-05-18.
//
#include <iostream>
#include <memory>

#include "sso_client.h"
#include "utils.h"

using namespace common;

std::shared_ptr<SSOClient> client;

/**
 * 性能压测
 * @return
 */
int main()
{
    // 1.初始化客户端
    std::string app_code("privilege-web");          //cmdb服务名
    std::string aes_key("vtrqMI5mR5urODuG");        //AES密钥
    std::string aes_iv("9686128666182599");         //AES偏移量
    client = std::make_shared<SSOClient>(app_code, aes_key, aes_iv);

    // 校验参数
    std::string request_url = "http://km.vivo.xyz/pages/viewpage.action?pageId=128195702";   //请求url
    std::map<std::string, std::string> request_headers;         //请求头
    request_headers["X-Forwarded-For"] = "10.13.184.104";       //ip与token一一对应
    request_headers["Cookie"] = "uuc-token=111; token=vF5r_KvGXkyqMMK4TB6.kOXTWpJL_xdRE8ZF7oYLfETWF79WLR_m0YENYzZily7aM6pD5rz6jcI*;";
    std::string redirect_url = "http://local.privilege.vivo.xyz:8086/privilege/main";   //验证不过，期望跳转url

    // 循环次数
    int loop = 10;
    SSOResult result;
    // 登录校验
    long start = SSOTimeUtils::GetCurrentTimeMicros();
    for (int i = 0; i < loop; ++i) {
        result = client->CheckLoginStatus(request_url, request_headers, redirect_url);
    }
    long end = SSOTimeUtils::GetCurrentTimeMicros();
    std::cout << loop << "次, 登录校验用时：" << (end-start) << "微秒\t" << (end-start)/1000 << " 毫秒" << std::endl;

    // 退出校验
    start = SSOTimeUtils::GetCurrentTimeMicros();
    for (int i = 0; i < loop; ++i) {
        result = client->DoLogoutStep(request_url, request_headers, redirect_url);
    }
    end = SSOTimeUtils::GetCurrentTimeMicros();
    std::cout << loop << "次, 退出校验用时：" << (end-start) << "微秒\t" << (end-start)/1000 << " 毫秒" << std::endl;

    return 0;
}