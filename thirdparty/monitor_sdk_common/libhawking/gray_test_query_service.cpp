//
// Created by 陈嘉豪 on 2019/12/4.
//

#include "thirdparty/monitor_sdk_common/libhawking/gray_test_query_service.h"

using namespace common;

std::shared_ptr<TestPlanQuery> GrayTestQueryService::QueryTestPlan(std::string &module_code, std::string &test_code,
        std::map<std::string, std::string> &filter) {
    // 参数校验
    std::shared_ptr<TestQueryResult> test_query_result = GrayTestCacheManager::GetGrayTest(module_code, test_code);
    
    if (test_query_result->result.code != RetCode::OK.code) {
        GrayTestLogUtils::LogError("failed to get test, test code is " + test_code + ", result code is " + \
                std::to_string(test_query_result->result.code) + ", result message is " + \
                test_query_result->result.message);
        return nullptr;
    }
    
    std::shared_ptr<GrayTest> gray_test = test_query_result->gray_test;
    std::shared_ptr<TestPlanQuery> result = GetPlanFromWhiteList(gray_test, filter);
    if (result != nullptr) {
        return result;
    }
    return GetPlanByStrategy(gray_test, filter);
}

std::shared_ptr<TestPlanQuery> GrayTestQueryService::GetPlanFromWhiteList(std::shared_ptr<GrayTest> gray_test,
        const std::map<std::string, std::string>& filter) {
    if (gray_test == nullptr) {
        return nullptr;
    }
    
    // 不存在白名单配置
    if (gray_test->device_uid2plan_id.empty()) {
        return nullptr;
    }
    
    // 不存在设备标识符 或 未匹配上白名单
    std::string device_uid = DeviceUidProvider::GetDeviceUid(filter);
    if (device_uid.empty() || !gray_test->device_uid2plan_id.count(device_uid)) {
        return nullptr;
    }
    
    long plan_id = gray_test->device_uid2plan_id[device_uid];
    std::shared_ptr<GrayTestPlan> white_list_plan;
    for (const auto &plan : gray_test->plans) {
        if (plan.id == plan_id) {
            white_list_plan = std::make_shared<GrayTestPlan>(plan);
            break;
        }
    }
    
    // 未查询到方案
    if (white_list_plan == nullptr) {
        return nullptr;
    }
    return GetTestPlanQuery(gray_test, white_list_plan->id, white_list_plan->params);
}

std::shared_ptr<TestPlanQuery> GrayTestQueryService::GetTestPlanQuery(std::shared_ptr<GrayTest> gray_test,
        long plan_id, const std::vector<Param>& params) {
    std::shared_ptr<TestPlanQuery> result = std::make_shared<TestPlanQuery>();
    result->testId = gray_test->id;
    result->testPlanId = plan_id;
    result->testCode = gray_test->code;
    result->refreshTime = gray_test->refresh_time;
    
    std::vector<ParamDto> param_dtos;
    for (auto &param : params) {
        ParamDto param_dto;
        param_dto.paramName = param.name;
        param_dto.paramValue = param.value;
        param_dtos.push_back(param_dto);
    }
    
    result->paramList = param_dtos;
    return result;
}

std::shared_ptr<TestPlanQuery> GrayTestQueryService::GetPlanByStrategy(std::shared_ptr<GrayTest> gray_test,
        const std::map<std::string, std::string>& filter) {
    if (gray_test == nullptr) {
        return nullptr;
    }
    std::shared_ptr<MatchStrategyParam> param = std::make_shared<MatchStrategyParam>();
    std::string test_key;
    test_key = gray_test->test_key;
    param->test_key = test_key;
    for (auto &plan : gray_test->plans) {
        if (IsHitPlan(plan, param, filter)) {
            return GetTestPlanQuery(gray_test, plan.id, plan.params);
        }
    }
    
    Json::Value root;
    for (const auto &iter : filter) {
        root[iter.first] = iter.second;
    }
    
    std::string filter_content;
    Json::StreamWriterBuilder stream_writer_builer;
    std::unique_ptr<Json::StreamWriter> stream_writer(stream_writer_builer.newStreamWriter());
    std::ostringstream os;
    
    stream_writer->write(root, &os);
    filter_content = os.str();
    GrayTestLogUtils::LogError("failed to match any plans, test code is " + gray_test->code + \
            ", filter content is " + filter_content);
    return nullptr;
}

bool GrayTestQueryService::IsHitPlan(const GrayTestPlan& plan, std::shared_ptr<MatchStrategyParam> param,
        const std::map<std::string, std::string>& filter) {
    // 优先级为0表示默认方案
    if (plan.priority == 0) {
        return true;
    }
    
    // 策略组间交集 组内交集
    for (const std::vector<Strategy>& strategies : plan.group_list) {
        if (!MatchStrategyGroup(strategies, param, filter)) {
            // 有一组匹配不上 该方案就匹配失败
            return false;
        }
    }
    return true;
}

bool GrayTestQueryService::MatchStrategyGroup(const std::vector<Strategy>& strategies,
        std::shared_ptr<MatchStrategyParam> param, const std::map<std::string, std::string>& filter) {
    for (const Strategy& strategy : strategies) {
        param->strategy = strategy;
        std::shared_ptr<StrategyExecutor> executor = StrategyExecutorManager::GetExecutor(strategy.type);
        if (executor != nullptr && executor->MatchStrategy(param, filter)) {
            return true;
        }
    }
    return false;
}
