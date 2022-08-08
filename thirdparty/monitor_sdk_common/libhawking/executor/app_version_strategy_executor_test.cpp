//
// Created by 陈嘉豪 on 2019/12/16.
//

#include "thirdparty/monitor_sdk_common/libhawking/executor/app_version_strategy_executor.h"
#include "thirdparty/monitor_sdk_common/libhawking/sdk_test_data_create_utils.h"

#include "thirdparty/gtest/gtest.h"

using namespace common;

class AppVersionStrategyExecutorTest : public testing::Test {
protected:
    virtual void SetUp() {
        executor = std::make_shared<AppVersionStrategyExecutor>();
    }
    virtual void TearDown() {}
    std::shared_ptr<AppVersionStrategyExecutor> executor;
};

TEST_F(AppVersionStrategyExecutorTest, support_strategy_type_TEST) {
    int type = executor->SupportStrategyType();
    EXPECT_EQ(2, type);
}

TEST_F(AppVersionStrategyExecutorTest, matchStrategy_version_not_exist_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetAppVersionStrategy(
            std::vector<std::string>{"1.1", "1.2", "1.3"});
    MatchStrategyParam param;
    param.strategy = strategy;
    std::map<std::string, std::string> filter;
    filter["packageName"] = "test";

    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(false, result);
}

TEST_F(AppVersionStrategyExecutorTest, matchStrategy_fail_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetAppVersionStrategy(
            std::vector<std::string>{"1.1", "1.2", "1.3"});
    MatchStrategyParam param;
    param.strategy = strategy;
    std::map<std::string, std::string> filter;
    filter["version"] = "2.1";

    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(false, result);
}

TEST_F(AppVersionStrategyExecutorTest, matchStrategy_success_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetAppVersionStrategy(
            std::vector<std::string>{"1.1", "1.2", "1.3"});
    MatchStrategyParam param;
    param.strategy = strategy;
    std::map<std::string, std::string> filter;
    filter["version"] = "1.1";

    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(true, result);
}

int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}