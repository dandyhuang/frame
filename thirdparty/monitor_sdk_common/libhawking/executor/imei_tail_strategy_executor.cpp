//
// Created by 陈嘉豪 on 2019/12/5.
//

#include "thirdparty/monitor_sdk_common/libhawking/executor/imei_tail_strategy_executor.h"

using namespace common;

int ImeiTailStrategyExecutor::SupportStrategyType() {
    return GrayTestConfig::strategy_type.IMEI_TAIL;
}

bool ImeiTailStrategyExecutor::MatchStrategy(std::shared_ptr<MatchStrategyParam> param,
        const std::map<std::string, std::string> &filter) {
    std::string imei;
    if (filter.count("imei")) {
        imei = filter.at("imei");
    }
    if (imei.empty()) {
        return false;
    }
    
    std::set<std::string> tail_set;
    SplitStringToSet(param->strategy.content, "|", &tail_set);
    imei = imei[imei.length()-1];
    return tail_set.count(imei) != 0;
}
