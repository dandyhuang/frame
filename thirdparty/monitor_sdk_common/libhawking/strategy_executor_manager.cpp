//
// Created by 陈嘉豪 on 2019/12/4.
//

#include "thirdparty/monitor_sdk_common/libhawking/strategy_executor_manager.h"

using namespace common;

std::map<int, std::shared_ptr<StrategyExecutor>> StrategyExecutorManager::strategy_executor_map{
    {GrayTestConfig::strategy_type.APP_TYPE, std::shared_ptr<StrategyExecutor>(new AppClientStrategyExecutor())},
    {GrayTestConfig::strategy_type.APP_VERSION, std::shared_ptr<StrategyExecutor>(new AppVersionStrategyExecutor())},
    {GrayTestConfig::strategy_type.IMEI_TAIL, std::shared_ptr<StrategyExecutor>(new ImeiTailStrategyExecutor())},
    {GrayTestConfig::strategy_type.IMEI_HASH, std::shared_ptr<StrategyExecutor>(new DeviceUidHashStrategyExecutor())},
    {GrayTestConfig::strategy_type.OPENID_HASH, std::shared_ptr<StrategyExecutor>(new OpenIdHashStrategyExecutor)}
};

std::shared_ptr<StrategyExecutor> StrategyExecutorManager::GetExecutor(int strategy_type) {
    std::shared_ptr<StrategyExecutor> strategy_executor = strategy_executor_map[strategy_type];
    if (strategy_executor == nullptr) {
        GrayTestLogUtils::LogError("Cannot find StrategyExecutor for strategy type: " + std::to_string(strategy_type));
    }
    return strategy_executor;
}