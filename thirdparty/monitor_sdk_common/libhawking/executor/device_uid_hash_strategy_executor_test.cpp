//
// Created by 陈嘉豪 on 2019/12/13.
//

#include "thirdparty/monitor_sdk_common/libhawking/executor/device_uid_hash_strategy_executor.h"
#include "thirdparty/monitor_sdk_common/libhawking/random_string_utils.h"
#include "thirdparty/monitor_sdk_common/libhawking/sdk_test_data_create_utils.h"

#include "thirdparty/gtest/gtest.h"

using namespace common;

class DeviceUidHashStrategyExecutorTest : public testing::Test {
protected:
    virtual void SetUp() {
        executor = std::make_shared<DeviceUidHashStrategyExecutor>();
        GrayTestLogUtils::Instance()->SetLogger(nullptr);
    }
    virtual void TearDown() {}
    std::shared_ptr<DeviceUidHashStrategyExecutor> executor;
};

TEST_F(DeviceUidHashStrategyExecutorTest, support_strategy_type_TEST) {
    EXPECT_EQ(5, executor->SupportStrategyType());
}

TEST_F(DeviceUidHashStrategyExecutorTest, matchStrategy_imei_hash_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetDeviceUidHashStrategy(std::vector<std::string>{"00", "01", "03"});
    MatchStrategyParam param;
    param.strategy = strategy;
    param.test_key = "testKey";
    
    std::string device_uid;
    while (true) {
        device_uid = RandomStringUtils::Generate(10);
        if (HashCodeUtils::GetHashOnePercent(param.test_key + device_uid, "MD5") == "00") {
            break;
        }
    }
    
    std::map<std::string, std::string> filter;
    filter["imei"] = device_uid;
    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(true, result);
}

TEST_F(DeviceUidHashStrategyExecutorTest, matchStrategy_vaid_hash_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetDeviceUidHashStrategy(std::vector<std::string>{"00", "01", "03"});
    MatchStrategyParam param;
    param.strategy = strategy;
    param.test_key = "testKey";
    
    std::string device_uid;
    while (true) {
        device_uid = RandomStringUtils::Generate(10);
        if (HashCodeUtils::GetHashOnePercent(param.test_key + device_uid, "MD5") == "00") {
            break;
        }
    }
    
    std::map<std::string, std::string> filter;
    filter["vaid"] = device_uid;
    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(true, result);
}

TEST_F(DeviceUidHashStrategyExecutorTest, matchStrategy_oaid_hash_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetDeviceUidHashStrategy(std::vector<std::string>{"00", "01", "03"});
    MatchStrategyParam param;
    param.strategy = strategy;
    param.test_key = "testKey";
    
    std::string device_uid;
    while (true) {
        device_uid = RandomStringUtils::Generate(10);
        if (HashCodeUtils::GetHashOnePercent(param.test_key + device_uid, "MD5") == "00") {
            break;
        }
    }
    
    std::map<std::string, std::string> filter;
    filter["oaid"] = device_uid;
    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(true, result);
}

TEST_F(DeviceUidHashStrategyExecutorTest, matchStrategy_aaid_hash_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetDeviceUidHashStrategy(std::vector<std::string>{"00", "01", "03"});
    MatchStrategyParam param;
    param.strategy = strategy;
    param.test_key = "testKey";
    
    std::string device_uid;
    while (true) {
        device_uid = RandomStringUtils::Generate(10);
        if (HashCodeUtils::GetHashOnePercent(param.test_key + device_uid, "MD5") == "00") {
            break;
        }
    }
    
    std::map<std::string, std::string> filter;
    filter["aaid"] = device_uid;
    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(true, result);
}

TEST_F(DeviceUidHashStrategyExecutorTest, matchStrategy_imei_hash_first_TEST) {
    Strategy strategy = SdkTestDataCreateUtils::GetDeviceUidHashStrategy(std::vector<std::string>{"00", "01", "03"});
    MatchStrategyParam param;
    param.strategy = strategy;
    param.test_key = "testKey";
    
    std::string device_uid;
    while (true) {
        device_uid = RandomStringUtils::Generate(10);
        if (HashCodeUtils::GetHashOnePercent(param.test_key + device_uid, "MD5") == "00") {
            break;
        }
    }
    
    std::map<std::string, std::string> filter;
    filter["imei"] = device_uid;
    filter["oaid"] = "test";
    bool result = executor->MatchStrategy(std::make_shared<MatchStrategyParam>(param), filter);
    
    EXPECT_EQ(true, result);
}

int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}
