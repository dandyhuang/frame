//
// Created by 陈嘉豪 on 2019/12/5.
//

#ifndef COMMON_LIBHAWKING_APP_VERSION_STRATEGY_EXECUTOR_H
#define COMMON_LIBHAWKING_APP_VERSION_STRATEGY_EXECUTOR_H

#include <map>
#include <set>
#include <string>

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_config.h"
#include "thirdparty/monitor_sdk_common/libhawking/strategy_executor.h"

namespace common {

class AppVersionStrategyExecutor : public StrategyExecutor {
public:
    AppVersionStrategyExecutor() = default;
    virtual ~AppVersionStrategyExecutor() {}
    
    int SupportStrategyType() override;
    
    bool MatchStrategy(std::shared_ptr<MatchStrategyParam> param,
            const std::map<std::string, std::string>& filter) override;
};

} // namespace common

#endif //COMMON_LIBHAWKING_APP_VERSION_STRATEGY_EXECUTOR_H
