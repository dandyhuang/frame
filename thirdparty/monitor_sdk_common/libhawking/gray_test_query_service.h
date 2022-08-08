//
// Created by 陈嘉豪 on 2019/12/4.
//

#ifndef COMMON_LIBHAWKING_GRAY_TEST_QUERY_SERVICE_H
#define COMMON_LIBHAWKING_GRAY_TEST_QUERY_SERVICE_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/libhawking/device_uid_provider.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_cache_manager.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_config.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_log_utils.h"
#include "thirdparty/monitor_sdk_common/libhawking/item.h"
#include "thirdparty/monitor_sdk_common/libhawking/strategy_executor_manager.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_plan_query.h"

#include "thirdparty/jsoncpp/json.h"

namespace common {

class GrayTestQueryService {
public:
    static std::shared_ptr<TestPlanQuery> QueryTestPlan(std::string &module_code,
            std::string &test_code, std::map<std::string, std::string> &filter);
    
    static std::shared_ptr<TestPlanQuery> GetPlanFromWhiteList(std::shared_ptr<GrayTest> gray_test,
            const std::map<std::string, std::string>& filter);
    
    static std::shared_ptr<TestPlanQuery> GetTestPlanQuery(std::shared_ptr<GrayTest> gray_test, long plan_id,
            const std::vector<Param>& params);
    
    static std::shared_ptr<TestPlanQuery> GetPlanByStrategy(std::shared_ptr<GrayTest> gray_test,
            const std::map<std::string, std::string>& filter);
    
    static bool IsHitPlan(const GrayTestPlan& plan, std::shared_ptr<MatchStrategyParam> param,
            const std::map<std::string, std::string>& filter);
    
    static bool MatchStrategyGroup(const std::vector<Strategy>& strategies, std::shared_ptr<MatchStrategyParam> param,
            const std::map<std::string, std::string>& filter);
};

} // namespace common

#endif //COMMON_LIBHAWKING_GRAY_TEST_QUERY_SERVICE_H
