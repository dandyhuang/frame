//
// Created by 陈嘉豪 on 2019/12/5.
//

#include "thirdparty/monitor_sdk_common/libhawking/executor/open_id_hash_strategy_executor.h"

using namespace common;

int OpenIdHashStrategyExecutor::SupportStrategyType() {
    return GrayTestConfig::strategy_type.OPENID_HASH;
}

bool OpenIdHashStrategyExecutor::MatchStrategy(std::shared_ptr<MatchStrategyParam> param,
        const std::map<std::string, std::string> &filter) {
    std::string open_id;
    std::string test_key = param->test_key;
    
    if (filter.count("openid")) {
        open_id = filter.at("openid");
    }
    if (open_id.empty() || test_key.empty()) {
        return false;
    }
    
    std::string distribute_factor = test_key + open_id;
    std::string hash_result_str;
    std::vector<std::string> hash_result;
    
    // 判断是取哈希值的后三位还是后两位，再匹配
    SplitString(param->strategy.content, "|", &hash_result);
    if (!hash_result.empty()) {
        if (hash_result[0].length() == 2) {
            hash_result_str = HashCodeUtils::GetHashOnePercent(distribute_factor, GrayTestConfig::hash_method);
        } else if (hash_result[0].length() == 3) {
            hash_result_str = HashCodeUtils::GetHashOneThousandth(distribute_factor, GrayTestConfig::hash_method);
        } else {
            return false;
        }
    } else {
        return false;
    }

    GrayTestLogUtils::LogDebug("openid is " + open_id + ", hash result = " + hash_result_str);
    
    std::set<std::string> hash_set(hash_result.begin(), hash_result.end());
    
    return hash_set.count(hash_result_str) != 0;
}
