//
// Created by 陈嘉豪 on 2019/11/29.
//

#ifndef COMMON_LIBHAWKING_GRAY_TEST_CONFIG_H
#define COMMON_LIBHAWKING_GRAY_TEST_CONFIG_H

#include <string>

namespace common {

struct SdkUrl {
    std::string remote_test_url = "/api/grayTest/queryAllBusinessTest.do";
};

struct RefreshCacheType {
    int init = 0;
    int task = 1;
};

struct StrategyType {
    //客户端策略
    int APP_TYPE = 1;
    //版本策略
    int APP_VERSION = 2;
    //指定imei尾数策略
    int IMEI_TAIL = 4;
    //哈希imei尾数策略
    int IMEI_HASH = 5;
    //哈希openid尾数策略
    int OPENID_HASH = 6;
};

struct TestEffType {
    int TIMING = 1;
};

//接口上传的filter的key名称
struct FilterKey {
    std::string IMEI = "imei";
    std::string VAID = "vaid";
    std::string OAID = "oaid";
    std::string AAID = "aaid";
};

class GrayTestConfig {
public:
    static SdkUrl sdk_url;
    
    static RefreshCacheType refresh_cache_type;
    
    static StrategyType strategy_type;
    
    static TestEffType test_eff_type;
    
    static FilterKey filter_key;
    
    static std::string hash_method;
};

} // namespace common

#endif //COMMON_LIBHAWKING_GRAY_TEST_CONFIG_H
