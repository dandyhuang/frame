//
// Created by 陈嘉豪 on 2019/12/4.
//

#ifndef COMMON_LIBHAWKING_STRATEGY_EXECUTOR_MANAGER_H
#define COMMON_LIBHAWKING_STRATEGY_EXECUTOR_MANAGER_H

#include <map>
#include <memory>

#include "thirdparty/monitor_sdk_common/libhawking/executor/app_client_strategy_executor.h"
#include "thirdparty/monitor_sdk_common/libhawking/executor/app_version_strategy_executor.h"
#include "thirdparty/monitor_sdk_common/libhawking/executor/device_uid_hash_strategy_executor.h"
#include "thirdparty/monitor_sdk_common/libhawking/executor/imei_tail_strategy_executor.h"
#include "thirdparty/monitor_sdk_common/libhawking/executor/open_id_hash_strategy_executor.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_log_utils.h"
#include "thirdparty/monitor_sdk_common/libhawking/strategy_executor.h"

namespace common {

class StrategyExecutorManager {
public:
    StrategyExecutorManager() = default;
    
    ~StrategyExecutorManager() = default;
    
    static std::map<int, std::shared_ptr<StrategyExecutor>> strategy_executor_map;
    
    static std::shared_ptr<StrategyExecutor> GetExecutor(int strategy_type);
};


} // namespace common

#endif //COMMON_LIBHAWKING_STRATEGY_EXECUTOR_MANAGER_H
