//
// Created by 陈嘉豪 on 2019/11/28.
//

#ifndef COMMON_LIBHAWKING_QUERY_CLIENT_H
#define COMMON_LIBHAWKING_QUERY_CLIENT_H

#include <map>
#include <memory>
#include <string>

#include "thirdparty/monitor_sdk_common/libhawking/gray_test_plan_query.h"

namespace common {

class QueryClient {
public:
    virtual std::shared_ptr<TestPlanQuery> QueryTestPlan(std::string module_code,
            std::string test_code, std::map<std::string, std::string> filter) = 0;
    
    virtual ~QueryClient() {}
};

} // namespace common

#endif //COMMON_LIBHAWKING_QUERY_CLIENT_H
