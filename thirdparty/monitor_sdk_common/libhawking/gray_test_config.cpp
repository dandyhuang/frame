//
// Created by 陈嘉豪 on 2019/12/10.
//

#include "thirdparty/monitor_sdk_common/libhawking/gray_test_config.h"

using namespace common;

SdkUrl GrayTestConfig::sdk_url;

RefreshCacheType GrayTestConfig::refresh_cache_type;

StrategyType GrayTestConfig::strategy_type;

TestEffType GrayTestConfig::test_eff_type;

FilterKey GrayTestConfig::filter_key;

// 哈希算法：MD5、SHA1
std::string GrayTestConfig::hash_method{"MD5"};