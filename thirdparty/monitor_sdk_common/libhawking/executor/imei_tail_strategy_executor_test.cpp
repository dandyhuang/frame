//
// Created by 陈嘉豪 on 2019/12/16.
//

#include "thirdparty/monitor_sdk_common/libhawking/executor/imei_tail_strategy_executor.h"
#include "thirdparty/monitor_sdk_common/libhawking/sdk_test_data_create_utils.h"

#include "thirdparty/gtest/gtest.h"

using namespace common;

class ImeiTailStrategyExecutorTest : public testing::Test {
protected:
    virtual void SetUp() {
        executor = std::make_shared<ImeiTailStrategyExecutor>();
    }
    virtual void TearDown() {}
    std::shared_ptr<ImeiTailStrategyExecutor> executor;
};

TEST_F(ImeiTailStrategyExecutorTest, support_strategy_type_TEST) {
    int type = executor->SupportStrategyType();
    EXPECT_EQ(4, type);
}

TEST_F(ImeiTailStrategyExecutorTest, matchStrategy_device_uid_not_exist_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetDeviceUidTailStrategy(std::vector<std::string>{"0", "1"});
    MatchStrategyParam param;
    param.strategy = strategy;
    std::map<std::string, std::string> filter;
    
    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(false, result);
}

TEST_F(ImeiTailStrategyExecutorTest, matchStrategy_imei_tail_match_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetDeviceUidTailStrategy(std::vector<std::string>{"0", "1"});
    MatchStrategyParam param;
    param.strategy = strategy;
    std::map<std::string, std::string> filter;
    filter["imei"] = "10000";
    
    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(true, result);
}

TEST_F(ImeiTailStrategyExecutorTest, matchStrategy_vaid_tail_not_match_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetDeviceUidTailStrategy(std::vector<std::string>{"0", "1"});
    MatchStrategyParam param;
    param.strategy = strategy;
    std::map<std::string, std::string> filter;
    filter["vaid"] = "10000";
    
    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(false, result);
}

TEST_F(ImeiTailStrategyExecutorTest, matchStrategy_oaid_tail_not_match_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetDeviceUidTailStrategy(std::vector<std::string>{"0", "1"});
    MatchStrategyParam param;
    param.strategy = strategy;
    std::map<std::string, std::string> filter;
    filter["oaid"] = "10000";
    
    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(false, result);
}

TEST_F(ImeiTailStrategyExecutorTest, matchStrategy_aaid_tail_not_match_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetDeviceUidTailStrategy(std::vector<std::string>{"0", "1"});
    MatchStrategyParam param;
    param.strategy = strategy;
    std::map<std::string, std::string> filter;
    filter["aaid"] = "10000";
    
    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(false, result);
}

TEST_F(ImeiTailStrategyExecutorTest, matchStrategy_imei_tail_match_first_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetDeviceUidTailStrategy(std::vector<std::string>{"0", "1"});
    MatchStrategyParam param;
    param.strategy = strategy;
    std::map<std::string, std::string> filter;
    filter["imei"] = "100000";
    filter["vaid"] = "100001";
    filter["oaid"] = "100002";
    filter["aaid"] = "100003";
    
    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(true, result);
}

int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}