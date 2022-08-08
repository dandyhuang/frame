//
// Created by 陈嘉豪 on 2019/11/28.
//

#ifndef COMMON_LIBHAWKING_GRAY_TEST_PLAN_QUERY_H
#define COMMON_LIBHAWKING_GRAY_TEST_PLAN_QUERY_H

#include <string>
#include <vector>

namespace common {

struct ParamDto {
    std::string paramName;
    std::string paramValue;
};

struct TestPlanQuery {
    std::string testCode;
    long testId;
    long testPlanId;
    int state;
    int refreshTime;
    std::vector<ParamDto> paramList;
};

} // namespace common

#endif //COMMON_LIBHAWKING_GRAY_TEST_PLAN_QUERY_H
