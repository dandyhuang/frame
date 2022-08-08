//
// Created by 陈嘉豪 on 2019/12/4.
//

#ifndef COMMON_LIBHAWKING_APP_CLIENT_STRATEGY_EXECUTOR_H
#define COMMON_LIBHAWKING_APP_CLIENT_STRATEGY_EXECUTOR_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_config.h"
#include "thirdparty/monitor_sdk_common/libhawking/item.h"
#include "thirdparty/monitor_sdk_common/libhawking/strategy_executor.h"

namespace common {

class AppClientStrategyExecutor : public StrategyExecutor {
public:
    AppClientStrategyExecutor() = default;
    virtual ~AppClientStrategyExecutor() {}
    
    int SupportStrategyType() override;
    
    bool MatchStrategy(std::shared_ptr<MatchStrategyParam> param,
            const std::map<std::string, std::string>& filter) override;
    
    bool appoint(const std::vector<ClientStrategy>& client_strategies,
            const std::map<std::string, std::string>& filter, const std::string& package_name);
    
    bool forbid(const std::vector<ClientStrategy>& client_strategies,
                 const std::map<std::string, std::string>& filter, const std::string& package_name);
    
    static std::string CONTENT_TYPE_FORBID;
};

} // namespace common

#endif //COMMON_LIBHAWKING_APP_CLIENT_STRATEGY_EXECUTOR_H
