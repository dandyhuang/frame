//
// Created by 陈嘉豪 on 2019/12/5.
//

#include "thirdparty/monitor_sdk_common/libhawking/executor/app_version_strategy_executor.h"

using namespace common;

int AppVersionStrategyExecutor::SupportStrategyType() {
    return GrayTestConfig::strategy_type.APP_VERSION;
}

bool AppVersionStrategyExecutor::MatchStrategy(std::shared_ptr<MatchStrategyParam> param,
        const std::map<std::string, std::string>& filter) {
    std::string version;
    if (filter.count("version")) {
        version = filter.at("version");
    }
    if (version.empty()) {
        return false;
    }
    // 根据策略ID查找客户端策略
    std::set<std::string> version_set;
    SplitStringToSet(param->strategy.content, "|", &version_set);
    return version_set.count(version) != 0;
}
