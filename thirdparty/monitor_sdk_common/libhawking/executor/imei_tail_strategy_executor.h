//
// Created by 陈嘉豪 on 2019/12/5.
//

#ifndef COMMON_LIBHAWKING_IMEI_TAIL_STRATEGY_EXECUTOR_H
#define COMMON_LIBHAWKING_IMEI_TAIL_STRATEGY_EXECUTOR_H

#include <map>
#include <memory>
#include <set>
#include <string>

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_config.h"
#include "thirdparty/monitor_sdk_common/libhawking/item.h"
#include "thirdparty/monitor_sdk_common/libhawking/strategy_executor.h"

namespace common {

// 指定imei尾数策略
class ImeiTailStrategyExecutor : public StrategyExecutor {
public:
    ImeiTailStrategyExecutor() = default;
    virtual ~ImeiTailStrategyExecutor() {}
    
    int SupportStrategyType() override;
    
    bool MatchStrategy(std::shared_ptr<MatchStrategyParam> param,
            const std::map<std::string, std::string>& filter) override;
};

} // namespace common

#endif //COMMON_LIBHAWKING_IMEI_TAIL_STRATEGY_EXECUTOR_H
