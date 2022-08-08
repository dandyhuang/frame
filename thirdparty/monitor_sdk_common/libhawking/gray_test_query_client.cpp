//
// Created by 陈嘉豪 on 2019/11/28.
//

#include "thirdparty/monitor_sdk_common/libhawking/gray_test_query_client.h"

#include "thirdparty/gflags/gflags.h"

using namespace common;

DEFINE_uint32(remote_fetch_task_delay_second, 3600, "delay seconds for remote fetching task");

GrayTestQueryClient::GrayTestQueryClient(std::string business_code, std::string domain, std::string env, bool init,
        std::shared_ptr<GrayTestLogger> logger) : business_code_(business_code), domain_(domain), env_(env) {
    SetLogger(logger);
    if (init) {
        Init();
    }
}

// 初始化日志，如果参数为空，则用默认日志配置
void GrayTestQueryClient::SetLogger(std::shared_ptr<GrayTestLogger> logger) {
    GrayTestLogUtils::Instance()->SetLogger(logger);
}

void GrayTestQueryClient::Init() {
    // 初始化任务调度器
    InitializeScheduledExecutor();
    // 获取远端实验配置，刷新缓存
    RefreshCache(GrayTestConfig::refresh_cache_type.init);
    // 启动定时任务
    StartRemoteFetchTask(FLAGS_remote_fetch_task_delay_second);
}

bool GrayTestQueryClient::IsStartDependOn() {
    return env_ != "dev" && env_ != "test";
}

void GrayTestQueryClient::InitializeScheduledExecutor() {
    int pool_size = 5;
    scheduled_executor_ = std::make_shared<GrayTestScheduledExecutor>(pool_size);
}

void GrayTestQueryClient::RefreshCache(int refresh_type) {
    try {
        std::shared_ptr<GrayTestRemoteResult> result = std::make_shared<GrayTestRemoteResult>();
        bool ret = RemoteFetch(result);
        if (!ret) {
            GrayTestLogUtils::LogError("failed to get remote gray test config");
            if (refresh_type == GrayTestConfig::refresh_cache_type.init && IsStartDependOn()) {
                throw GrayTestSdkStartException();
            }
        } else {
            // 成功获取到远端配置，写入缓存
            GrayTestCacheManager::Refresh(result->business, result->modules);
        }
    } catch (GrayTestSdkStartException& e) {
        throw e;
    } catch (std::exception& e) {
        if (refresh_type == GrayTestConfig::refresh_cache_type.init) {
            GrayTestLogUtils::LogError("failed to get remote test config when starting");
            if (IsStartDependOn()) {
                throw GrayTestSdkStartException();
            }
        } else {
            GrayTestLogUtils::LogError("failed to get remote test config when refreshing");
        }
    }
}

void GrayTestQueryClient::StartRemoteFetchTask(int delay_second) {
    scheduled_executor_->ScheduleWithFixedDelay([this] (const bool *terminated) {
        if (*terminated) return;
        RefreshCache(GrayTestConfig::refresh_cache_type.task);
    }, delay_second * 1000, delay_second * 1000);
}

// 使用http获取远端配置，尝试3次
bool GrayTestQueryClient::RemoteFetch(std::shared_ptr<GrayTestRemoteResult> gray_test_remote_result) {
    std::string url = domain_ + GrayTestConfig::sdk_url.remote_test_url;
    url += "?businessCode=" + business_code_;
    HttpClient http_client;
    HttpClient::Options options;
    HttpClient::ErrorCode error_code;
    HttpResponse response;
    
    std::string user_agent = "vivohawking-client-cpp";
    options.AddHeader("User-Agent", user_agent);
    
    for (int i = 0; i < 3; i++) {
        if (!http_client.Get(url, options, &response, &error_code)) {
            GrayTestLogUtils::LogError("failed to get remote test config, error msg: " + \
                                       (std::string)(HttpClient::GetErrorMessage(error_code)));
            continue; // 重试
        } else {
            const std::string& json_str = response.Body();
            Json::CharReaderBuilder builder;
            scoped_ptr<Json::CharReader> reader(builder.newCharReader());
            Json::Value root;
            if (!reader->parse(json_str.c_str(), json_str.c_str() + json_str.size(), &root, nullptr)) {
                return false;
            }
            
            int retcode = root["code"].asInt();
            if (retcode != RetCode::OK.code) {
                return false;
            }
            
            // 按照层级从Json读取数据
            Json::Value data = root["data"];
            Json::Value business = data["business"];
            Json::Value modules = data["modules"];
            
            Business new_business;
            new_business.name = business["name"].asString();
            new_business.mark = business["mark"].asString();
            new_business.refresh_time = business["refreshTime"].asInt();
            gray_test_remote_result->business = std::make_shared<Business>(new_business);
            
            for (Json::Value &module : modules) {
                Module new_module;
                new_module.business_id = module["businessId"].asInt();
                new_module.code = module["code"].asString();
                new_module.name = module["name"].asString();
                for (Json::Value &gray_test : module["tests"]) {
                    GrayTest new_gray_test;
                    new_gray_test.id = long(gray_test["id"].asUInt64());
                    new_gray_test.code = gray_test["code"].asString();
                    new_gray_test.name = gray_test["name"].asString();
                    new_gray_test.test_key = gray_test["testKey"].asString();
                    new_gray_test.eff_type = gray_test["effType"].asInt();
                    new_gray_test.eff_time = long(gray_test["effTime"].asUInt64());
                    new_gray_test.exp_time = long(gray_test["expTime"].asUInt64());
                    new_gray_test.refresh_time = gray_test["refreshTime"].asInt();
                    Json::Value::Members members = gray_test["deviceUid2PlanId"].getMemberNames();
                    for (Json::Value::Members::iterator member = members.begin(); member != members.end(); member++) {
                        std::string key = *member;
                        new_gray_test.device_uid2plan_id.insert(
                                std::make_pair(key, long(gray_test["deviceUid2PlanId"][key].asUInt64())));
                    }
                    for (Json::Value &gray_test_plan : gray_test["plans"]) {
                        GrayTestPlan new_gray_test_plan;
                        new_gray_test_plan.id = long(gray_test_plan["id"].asUInt64());
                        new_gray_test_plan.name = gray_test_plan["name"].asString();
                        new_gray_test_plan.priority = gray_test_plan["priority"].asInt();
                        for(Json::Value &param : gray_test_plan["params"]) {
                            Param new_param;
                            new_param.name = param["name"].asString();
                            new_param.value = param["value"].asString();
                            new_gray_test_plan.params.push_back(new_param);
                        }
                        for (Json::Value &strategy_group : gray_test_plan["groupList"]) {
                            std::vector<Strategy> new_strategy_group;
                            for (Json::Value &strategy : strategy_group) {
                                Strategy new_strategy;
                                new_strategy.id = long(strategy["id"].asUInt64());
                                new_strategy.type = strategy["type"].asInt();
                                new_strategy.content = strategy["content"].asString();
                                for (Json::Value &client_strategy : strategy["clientStrategy"]) {
                                    ClientStrategy new_client_strategy;
                                    new_client_strategy.package_name = client_strategy["packageName"].asString();
                                    new_client_strategy.version = client_strategy["version"].asString();
                                    new_strategy.client_strategies.push_back(new_client_strategy);
                                }
                                new_strategy_group.push_back(new_strategy);
                            }
                            new_gray_test_plan.group_list.push_back(new_strategy_group);
                        }
                        new_gray_test.plans.push_back(new_gray_test_plan);
                    }
                    new_module.tests.push_back(new_gray_test);
                }
                gray_test_remote_result->modules.push_back(std::make_shared<Module>(new_module));
            }
            return true;
        }
    }
    GrayTestLogUtils::LogError("failed to get remote test config after 3 times");
    return false;
}

std::shared_ptr<TestPlanQuery> GrayTestQueryClient::QueryTestPlan(std::string module_code,
        std::string test_code, std::map<std::string, std::string> filter) {
    return GrayTestQueryService::QueryTestPlan(module_code, test_code, filter);
}
