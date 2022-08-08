## 单点登录 C++ SDK

接口说明wiki   
<http://km.vivo.xyz/pages/viewpage.action?pageId=198056562>

### 1. 接口类初始化

```c++
SSOClient(const std::string& app_code,
          const std::string& aes_key,
          const std::string& aes_iv);
```
每个appCode都有唯一的客户端需要初始化。

| 参数名称  | 是否必填 | 含义 | 备注 |
|---|---|---|---|
| app_code | 是 | cmdb节点服务名 |  |
| aes_key | 是 | AES密钥 | 每个cmdb服务节点有唯一的密钥和偏移量 |
| aes_iv | 是 | AES偏移量 | |


### 2. 接口函数

```c++
// 检查登录状态
ResultVo CheckLoginStatus(const std::string& request_url, std::map<std::string, std::string> request_headers, const std::string& redirect_url);
 
// 执行退出操作
ResultVo DoLogoutStep(const std::string& request_url, std::map<std::string, std::string> request_headers, const std::string& redirect_url);
```

#### 2.1. 输入参数

| 参数名称 | 含义 | 备注 |
|---|---|---|
| request_url | 业务请求url | Request信息 |
| request_headers | 业务请求头信息 |Request信息<br>map存储，需要传入完整的头信息<br>登录校验关键的头信息："X-Forwarded-For"和"Cookie"|
| redirect_url | 校验不过，期望跳转的页面| |

#### 2.2. 返回参数
接口返回一个结果类：SSOResult。

| 参数名称 | 含义 | 备注 |
|---|---|---|
| success | 登录/退出校验是否成功 |  |
| code | 返回码 |0: 成功 <br> -1: 参数校验失败<br> -9999: 需要重定向|
| redirect_url | 校验不过，期望跳转的页面| |

##### a. 登录校验
有3种情况：
<br>成功（0）：无需额外操作，进入业务逻辑即可
<br>参数校验失败（-1）：检查输入参数，包括初始化参数
<br>重定向（-9999）：跳转登录
只有返回码为0，success为true。

##### b. 退出校验
有2种情况：
<br>参数校验失败（-1）：检查输入参数，包括初始化参数
<br>重定向（-9999）：跳转登录
返回码为-9999（重定向），success为true。退出的时候成功一定会返回重定向页面。

### 3. 接口使用方法
单点登录/退出验证的关键信息在于：token信息
其中token与登录的ip地址有关，每天token值都会更新。
使用测试例子的时候，需要将ip和token替换。
```c++
#include "sso_client.h"
 
using namespace common;
 
std::shared_ptr<SSOClient> client;               //校验客户端
 
int main(){
    // client初始化
    std::string app_code("privilege-web");          //cmdb服务名
    std::string aes_key("vtrqMI5mR5urODuG");        //AES密钥
    std::string aes_iv("9686128666182599");         //AES偏移量
    client = std::make_shared<SSOClient>(app_code, aes_key, aes_iv);
 
    // 1.登录校验
    // 请求url
    std::string request_url = "http://km.vivo.xyz/pages/viewpage.action?pageId=128195702";
    // 请求headers
    std::map<std::string, std::string> request_headers;
    request_headers["X-Forwarded-For"] = "10.13.184.104";    //ip与token一一对应
    request_headers["Cookie"] = "uuc-token=111; token=vF5r_KvGXkyqMMK4TB6.kAWnO8vqJL9r7GVqcKYKvsrYg9L9uDZI0G6BYfksxar4bsbxHRe8Hbc*;";
    // 校验不过，希望跳转的url
    std::string redirect_url = "http://local.privilege.vivo.xyz:8086/privilege/main";
 
    ResultVo result = client->CheckLoginStatus(request_url, request_headers, redirect_url);
    std::cout << "登录校验结果：" << result.ToString() << std::endl;
 
    // 2.退出校验
    std::string request_url2 = "http://km.vivo.xyz/pages/viewpage.action?token=vF5r_KvGXkyqMMK4TB6.kAWnO8vqJL9r7GVqcKYKvsrYg9L9uDZI0G6BYfksxar4bsbxHRe8Hbc*";
    // 请求headers
    std::map<std::string, std::string> request_headers2;
    result = client->DoLogoutStep(request_url2, request_headers2, redirect_url);
 
    std::cout << "\n退出校验结果："  << result.ToString() << std::endl;
 
    return 0;
}
```