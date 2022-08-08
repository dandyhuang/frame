//
// Created by 陈嘉豪 on 2019/12/4.
//

#ifndef COMMON_LIBHAWKING_STRATEGY_EXECUTOR_H
#define COMMON_LIBHAWKING_STRATEGY_EXECUTOR_H

#include <map>
#include <string>

#include "thirdparty/monitor_sdk_common/libhawking/item.h"

namespace common {

class StrategyExecutor {
public:
    virtual ~StrategyExecutor() {}
    
    virtual int SupportStrategyType() = 0;
    
    virtual bool MatchStrategy(std::shared_ptr<MatchStrategyParam> param,
            const std::map<std::string, std::string>& filter) = 0;
};

} // namespace common

#endif //COMMON_LIBHAWKING_STRATEGY_EXECUTOR_H
