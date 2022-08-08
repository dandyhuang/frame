## Hawking实验平台C++ SDK

接口说明wiki   
<http://km.vivo.xyz/pages/viewpage.action?pageId=130872734>

### 1. 接口类初始化

```c++
GrayTestQueryClient(std::string business_code, 
                    std::string domain, 
                    std::string env, 
                    bool init = true,
                    std::shared_ptr<GrayTestLogger> logger = nullptr);
```

| 参数名称  | 是否必填 | 含义 | 备注 |
|---|---|---|---|
| business_code | 是 | 业务编码，由实验平台分配 | com.vivo.browser |
| domain | 是 | 实验平台实验查询接口的地址前缀，<br>可区分测试环境、预上线环境、线上环境 | 测试环境地址： http://10.101.103.80:8080<br>预上线环境地址： http://gray-pre.vivo.com.cn<br>线上环境地址： http://gray.vivo.com.cn |
| env | 是 | 业务进程所在的环境，开发环境和测试环境下，<br>程序初始化失败不会断开，但是预上线环境和<br>线上环境初始化失败会断开程序 | 开发环境：dev<br>测试环境：test<br>预上线环境：pre<br>线上环境：prd|
| init | 否 | 是否初始化SDK | 仅供测试用，正式使用一定需要初始化 |
| logger | 否 | 日志配置类，不填的话使用默认日志配置 |

### 2. 接口函数

```c++
std::shared_ptr<TestPlanQuery> QueryTestPlan(std::string module_code, 
                                             std::string test_code, 
                                             std::map<std::string, std::string> filter);
```

#### 2.1. 输入参数

| 参数名称 | 含义 |
|---|---|
| module_code | 模块编码 |
| test_code | 实验编码 |
| filter | 过滤条件<br>map里面的key：packageName, version, imei, openid, vaid, oaid, aaid。<br>其中imei, vaid, oaid, aaid 只需要传入一个即可。优先级依次降低，<br>使用其作为唯一标识符进行分流计算|

#### 2.2. 返回参数
接口返回结果是一个智能指针，指向TestPlanQuery结构体。
```c++
struct TestPlanQuery {
    std::string testCode;
    long testId;
    long testPlanId;
    int state;
    int refreshTime;
    std::vector<ParamDto> paramList;
};

struct ParamDto {
    std::string paramName;
    std::string paramValue;
};
```

### 3. 接口使用方法
使用范例。
```c++
int main(int argc, char ** argv) {
    // 预先指定好必填项，并初始化客户端
    std::string business_code = "com.vivo.browser";
    std::string domain = "http://10.101.103.80:8080";
    std::string env = "dev";
    // 用智能指针来管理类
    std::shared_ptr<GrayTestQueryClient> client = std::make_shared<GrayTestQueryClient>(business_code, domain, env);
    
    // 定义过滤条件
    std::map<std::string, std::string> filter1{
        {"imei", "LjsTDfWiiv0"},
        {"version", "5.5.0"},
        {"openid", "mIqNaEHGUr0"}
    };
    
    // 返回结果也是智能指针，可判断是否空指针来确认是否获取到实验配置
    std::shared_ptr<TestPlanQuery> result = client->QueryTestPlan("M001", "2019111101", filter1);
    
    // 判断结果是否为空指针，如果是则没有获取到实验配置
    if (result == nullptr) {
        std::cout << "cannot get remote test result" << std::endl;
        return 0;
    }
    
    std::cout << "result1: test plan id: " << result->testPlanId << ", param name: " << result->paramList[0].paramName \
            << ", param value: " << result->paramList[0].paramValue << ", refresh time: " << result->refreshTime \
            << std::endl;
            
    return 0;
}
```