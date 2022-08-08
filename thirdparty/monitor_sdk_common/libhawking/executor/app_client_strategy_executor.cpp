//
// Created by 陈嘉豪 on 2019/12/4.
//

#include "thirdparty/monitor_sdk_common/libhawking/executor/app_client_strategy_executor.h"

using namespace common;

std::string AppClientStrategyExecutor::CONTENT_TYPE_FORBID = "1";

int AppClientStrategyExecutor::SupportStrategyType() {
    return GrayTestConfig::strategy_type.APP_TYPE;
}

bool AppClientStrategyExecutor::MatchStrategy(std::shared_ptr<MatchStrategyParam> param,
        const std::map<std::string, std::string>& filter) {
    std::string package_name;
    if (filter.count("packageName")) {
        package_name = filter.at("packageName");
    }
    if (package_name.empty()) {
        return false;
    }
    
    // 根据策略id查找客户端策略
    std::vector<ClientStrategy> client_strategies = param->strategy.client_strategies;
    std::string content = param->strategy.content;
    if (CONTENT_TYPE_FORBID == content) {
        return forbid(client_strategies, filter, package_name);
    } else {
        return appoint(client_strategies, filter, package_name);
    }
}

bool AppClientStrategyExecutor::appoint(const std::vector<ClientStrategy>& client_strategies,
        const std::map<std::string, std::string>& filter, const std::string& package_name) {
    for (const ClientStrategy& client_strategy : client_strategies) {
        std::set<std::string> version_set;
        SplitStringToSet(client_strategy.version, "|", &version_set);
        if (package_name == client_strategy.package_name) {
            if (client_strategy.version.empty()) {
                return true;
            }
            if (filter.count("version") && version_set.count(filter.at("version"))) {
                return true;
            }
        }
    }
    return false;
}

bool AppClientStrategyExecutor::forbid(const std::vector<ClientStrategy>& client_strategies,
        const std::map<std::string, std::string>& filter, const std::string& package_name) {
    for (const ClientStrategy& client_strategy : client_strategies) {
        std::set<std::string> version_set;
        SplitStringToSet(client_strategy.version, "|", &version_set);
        if (package_name == client_strategy.package_name) {
            if (client_strategy.version.empty()) {
                return false;
            }
            if (filter.count("version") && version_set.count(filter.at("version"))) {
                return false;
            }
        }
    }
    return true;
}
