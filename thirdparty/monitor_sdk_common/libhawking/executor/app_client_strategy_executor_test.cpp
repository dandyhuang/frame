//
// Created by 陈嘉豪 on 2019/12/16.
//

#include <sys/time.h>

#include "thirdparty/monitor_sdk_common/libhawking/executor/app_client_strategy_executor.h"
#include "thirdparty/monitor_sdk_common/libhawking/sdk_test_data_create_utils.h"

#include "thirdparty/gtest/gtest.h"

using namespace common;

class AppClientStrategyExecutorTest : public testing::Test {
protected:
    virtual void SetUp() {
        executor = std::make_shared<AppClientStrategyExecutor>();
    }
    virtual void TearDown() {}
    std::shared_ptr<AppClientStrategyExecutor> executor;
};

TEST_F(AppClientStrategyExecutorTest, support_strategy_type_TEST) {
    int type = executor->SupportStrategyType();
    EXPECT_EQ(1, type);
}

TEST_F(AppClientStrategyExecutorTest, matchStrategy_noPackageName_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetForbidAppClientStrategy();
    MatchStrategyParam param;
    param.strategy = strategy;
    std::map<std::string, std::string> filter;
    filter["imei"] = "imei";
    
    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(false, result);
}

TEST_F(AppClientStrategyExecutorTest, matchStrategy_reject_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetForbidAppClientStrategy();
    MatchStrategyParam param;
    param.strategy = strategy;
    std::map<std::string, std::string> filter;
    filter["packageName"] = strategy.client_strategies[0].package_name;
    filter["version"] = strategy.client_strategies[0].version;

    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(false, result);
}

TEST_F(AppClientStrategyExecutorTest, matchStrategy_not_reject_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetForbidAppClientStrategy();
    MatchStrategyParam param;
    param.strategy = strategy;
    std::map<std::string, std::string> filter;
    filter["packageName"] = strategy.client_strategies[0].package_name;
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long now = tv.tv_sec;
    filter["version"] = std::to_string(now);

    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(true, result);
}

TEST_F(AppClientStrategyExecutorTest, matchStrategy_specify_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetAppointAppClientStrategy();
    MatchStrategyParam param;
    param.strategy = strategy;
    std::map<std::string, std::string> filter;
    filter["packageName"] = strategy.client_strategies[0].package_name;
    filter["version"] = strategy.client_strategies[0].version;

    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(true, result);
}

TEST_F(AppClientStrategyExecutorTest, matchStrategy_specify_fail_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetAppointAppClientStrategy();
    MatchStrategyParam param;
    param.strategy = strategy;
    std::map<std::string, std::string> filter;
    filter["packageName"] = strategy.client_strategies[0].package_name;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    long now = tv.tv_sec;
    filter["version"] = std::to_string(now);

    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(false, result);
}

int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}