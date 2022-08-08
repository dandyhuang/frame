//
// Created by 陈嘉豪 on 2019/12/5.
//

#ifndef COMMON_LIBHAWKING_OPEN_ID_HASH_STRATEGY_EXECUTOR_H
#define COMMON_LIBHAWKING_OPEN_ID_HASH_STRATEGY_EXECUTOR_H

#include <map>
#include <memory>
#include <set>
#include <string>

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_config.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_log_utils.h"
#include "thirdparty/monitor_sdk_common/libhawking/hash_code_utils.h"
#include "thirdparty/monitor_sdk_common/libhawking/item.h"
#include "thirdparty/monitor_sdk_common/libhawking/strategy_executor.h"

namespace common {

class OpenIdHashStrategyExecutor : public StrategyExecutor {
public:
    OpenIdHashStrategyExecutor() = default;
    virtual ~OpenIdHashStrategyExecutor() {}
    
    int SupportStrategyType() override;
    
    bool MatchStrategy(std::shared_ptr<MatchStrategyParam> param,
            const std::map<std::string, std::string>& filter) override;
};

} // namespace common

#endif //COMMON_LIBHAWKING_OPEN_ID_HASH_STRATEGY_EXECUTOR_H
