//
// Created by 陈嘉豪 on 2019/12/16.
//

#include "thirdparty/monitor_sdk_common/libhawking/executor/open_id_hash_strategy_executor.h"
#include "thirdparty/monitor_sdk_common/libhawking/random_string_utils.h"
#include "thirdparty/monitor_sdk_common/libhawking/sdk_test_data_create_utils.h"

#include "thirdparty/gtest/gtest.h"

using namespace common;

class OpenIdHashStrategyExecutorTest : public testing::Test {
protected:
    virtual void SetUp() {
        executor = std::make_shared<OpenIdHashStrategyExecutor>();
        GrayTestLogUtils::Instance()->SetLogger(nullptr);
    }
    virtual void TearDown() {}
    std::shared_ptr<OpenIdHashStrategyExecutor> executor;
};

TEST_F(OpenIdHashStrategyExecutorTest, support_strategy_type_TEST) {
    EXPECT_EQ(6, executor->SupportStrategyType());
}

TEST_F(OpenIdHashStrategyExecutorTest, matchStrategy_open_id_not_exist_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetOpenidHashTailStrategy(std::vector<std::string>{"00", "01"});
    MatchStrategyParam param;
    param.strategy = strategy;
    param.test_key = "testKey";
    std::map<std::string, std::string> filter;
    
    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(false, result);
}

TEST_F(OpenIdHashStrategyExecutorTest, matchStrategy_hash_tail_match_fail_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetOpenidHashTailStrategy(std::vector<std::string>{"00", "01"});
    MatchStrategyParam param;
    param.strategy = strategy;
    param.test_key = "testKey";
    std::map<std::string, std::string> filter;
    filter["openid"] = "test";
    
    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(false, result);
}

TEST_F(OpenIdHashStrategyExecutorTest, matchStrategy_hash_tail_match_success_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetOpenidHashTailStrategy(std::vector<std::string>{"00", "01"});
    MatchStrategyParam param;
    param.strategy = strategy;
    param.test_key = "testKey";
    
    std::string openid;
    while (true) {
        openid = RandomStringUtils::Generate(10);
        if (HashCodeUtils::GetHashOnePercent(param.test_key + openid, "MD5") == "00") {
            break;
        }
    }
    std::map<std::string, std::string> filter;
    filter["openid"] = openid;
    
    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(true, result);
}

int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}
