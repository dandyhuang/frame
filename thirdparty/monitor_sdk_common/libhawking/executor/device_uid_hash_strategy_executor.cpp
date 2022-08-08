//
// Created by 陈嘉豪 on 2019/12/5.
//

#include "thirdparty/monitor_sdk_common/libhawking/executor/device_uid_hash_strategy_executor.h"

using namespace common;

int DeviceUidHashStrategyExecutor::SupportStrategyType() {
    return GrayTestConfig::strategy_type.IMEI_HASH;
}

bool DeviceUidHashStrategyExecutor::MatchStrategy(std::shared_ptr<MatchStrategyParam> param,
        const std::map<std::string, std::string> &filter) {
    std::string test_key = param->test_key;
    // 获取唯一标识符
    std::string device_uid = DeviceUidProvider::GetDeviceUid(filter);
    if (device_uid.empty() || test_key.empty()) {
        return false;
    }
    
    std::string distribute_factor = test_key + device_uid;
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

    std::string msg = "device UID is " + device_uid + ", hash result = " + hash_result_str;
    GrayTestLogUtils::LogDebug(msg);
    
    std::set<std::string> hash_set(hash_result.begin(), hash_result.end());
    return hash_set.count(hash_result_str) != 0;
}
