//
// Created by 陈嘉豪 on 2019/12/16.
//

#include <iostream>

#include "thirdparty/monitor_sdk_common/libhawking/gray_test_query_client.h"

using namespace common;

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
    
    std::map<std::string, std::string> filter2{
        {"imei", "test"},
        {"version", "5.5.0"},
        {"openid", "mIqNaEHGUr0"}
    };
    
    result = client->QueryTestPlan("M001", "2019111101", filter2);
    
    if (result == nullptr) {
        std::cout << "cannot get remote test result" << std::endl;
        return 0;
    }
    
    std::cout << "result2: test plan id: " << result->testPlanId << ", param name: " << result->paramList[0].paramName \
            << ", param value: " << result->paramList[0].paramValue << ", refresh time: " << result->refreshTime \
            << std::endl;
    
    return 0;
}