//
// Created by 陈嘉豪 on 2019/11/29.
//

#ifndef COMMON_LIBHAWKING_ITEM_H
#define COMMON_LIBHAWKING_ITEM_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/libhawking/ret_code.h"

namespace common {

struct ClientStrategy {
    std::string package_name;
    std::string version;
};

struct Strategy {
    long id;
    int type;
    std::string content;
    std::vector<ClientStrategy> client_strategies;
};

struct Param {
    std::string name;
    std::string value;
};

struct MatchStrategyParam {
    Strategy strategy;
    std::string test_key;
};

struct GrayTestPlan {
    long id;
    std::string name;
    int priority;
    std::vector<Param> params;
    std::vector<std::vector<Strategy>> group_list;
};

struct GrayTest {
    long id;
    std::string code;
    std::string name;
    std::string test_key;
    int eff_type; // 生效方式：0：立即生效 1：预约生效
    long eff_time; // 生效时间，时间戳
    long exp_time; // 失效时间, 时间戳
    int refresh_time;
    std::vector<GrayTestPlan> plans;
    std::map<std::string, long> device_uid2plan_id;
};

struct Business {
    std::string name;
    std::string mark;
    int refresh_time;
};

struct Module {
    int business_id;
    std::string name;
    std::string code;
    std::vector<GrayTest> tests;
};

struct GrayTestRemoteResult {
    std::shared_ptr<Business> business;
    std::vector<std::shared_ptr<Module>> modules;
};

struct TestQueryResult {
    retCode result;
    std::shared_ptr<GrayTest> gray_test;
    TestQueryResult(retCode ret_code) : result(ret_code) {}
    TestQueryResult(std::shared_ptr<GrayTest> new_gray_test) : result(RetCode::OK), gray_test(new_gray_test) {}
};

} // namespace common

#endif //COMMON_LIBHAWKING_ITEM_H
