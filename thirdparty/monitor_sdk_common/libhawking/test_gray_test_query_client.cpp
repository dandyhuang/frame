//
// Created by 陈嘉豪 on 2019/12/7.
//

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "thirdparty/monitor_sdk_common/libhawking/gray_test_query_client.h"
#include "thirdparty/monitor_sdk_common/libhawking/id_utils.h"
#include "thirdparty/monitor_sdk_common/libhawking/random_string_utils.h"
#include "thirdparty/monitor_sdk_common/libhawking/sdk_test_data_create_utils.h"

#include "thirdparty/gtest/gtest.h"

using namespace common;
using namespace std;

class GrayTestQueryClientTest : public testing::Test {
protected:
    virtual void SetUp() {
        string business_code = "com.vivo.browser";
        string domain = "http://10.101.103.80:8080";
        string env = "dev";
        
        client = std::make_shared<GrayTestQueryClient>(business_code, domain, env, false);
        client->RefreshCache(GrayTestConfig::refresh_cache_type.init);
    }
    
    virtual void TearDown() {
        client.reset();
    }
    
//    shared_ptr<QueryClient> client;
    shared_ptr<GrayTestQueryClient> client;
};

TEST_F(GrayTestQueryClientTest, filter1Test) {
    map<string, string> filter{
        {"imei", "LjsTDfWiiv0"},
        {"version", "5.5.0"},
        {"openid", "mIqNaEHGUr0"}
    };
    
    shared_ptr<TestPlanQuery> result = client->QueryTestPlan("M001", "2019111101", filter);
    
    if (result == nullptr) {
        FAIL();
    }
    
    EXPECT_EQ(8722, result->testPlanId);
    EXPECT_EQ("1", result->paramList[0].paramName);
    EXPECT_EQ("1", result->paramList[0].paramValue);
    EXPECT_EQ(60, result->refreshTime);
}

class GrayTestQueryClientTest2 : public testing::Test {
protected:
    virtual void SetUp() {
        client = std::make_shared<GrayTestQueryClient>("testbiz", "http://test", "test", false);
        
        std::shared_ptr<Business> business = std::make_shared<Business>();
        business->name = "business";
        business->refresh_time = 10;
        business->mark = "marl";
        
        std::shared_ptr<Module> module = std::make_shared<Module>();
        module->business_id = IdUtils::GetNextId();
        module->code = "moduleCode";
        module->name = "moduleName";
        
        std::vector<GrayTest> tests;
        LoadTests(&tests);
        module->tests = tests;
        
        GrayTestCacheManager::Refresh(business, std::vector<std::shared_ptr<Module>>{module});
    }
    
    virtual void TearDown() {
        client.reset();
    }
    
    void LoadTests(std::vector<GrayTest> *tests);
    
    void LoadPlans(GrayTest *test);
    
    std::shared_ptr<QueryClient> client;
};

void GrayTestQueryClientTest2::LoadTests(std::vector<GrayTest> *tests) {
    GrayTest test;
    test.id = IdUtils::GetNextId();
    test.code = "testCode";
    test.eff_type = 0;
    test.name = "testName";
    test.refresh_time = 60;
    test.test_key = "testKey";
    
    LoadPlans(&test);
    
    std::vector<GrayTestPlan> plans = test.plans;
    std::map<std::string, long> white_list;
    white_list.insert(std::make_pair("whiteList1", plans[0].id));
    white_list.insert(std::make_pair("whiteList2", plans[1].id));
    white_list.insert(std::make_pair("whiteList3", plans[2].id));
    test.device_uid2plan_id = white_list;
    
    tests->push_back(test);
}

void GrayTestQueryClientTest2::LoadPlans(GrayTest *test) {
    std::vector<GrayTestPlan> plans;
    for (int i = 2; i >= 0; --i) {
        GrayTestPlan plan;
        plan.priority = i;
        plan.id = IdUtils::GetNextId();
        plan.name = "plan" + std::to_string(i);
        
        std::vector<Param> params;
        Param param;
        param.name = "name";
        param.value = std::to_string(i);
        params.push_back(param);
        
        if (i == 2) {
            std::vector<std::vector<Strategy>> strategy_groups;
            
            //设备标志符尾号为00，hash尾号也为00
            std::vector<Strategy> imei_group;
            imei_group.push_back(SdkTestDataCreateUtils::GetDeviceUidTailStrategy(std::vector<std::string>{"0"}));
            imei_group.push_back(SdkTestDataCreateUtils::GetDeviceUidHashStrategy(std::vector<std::string>{"00"}));
            //指定客户端策略
            std::vector<Strategy> app_client;
            app_client.push_back(SdkTestDataCreateUtils::GetAppointAppClientStrategy());
            //openid hash尾号为00
            std::vector<Strategy> openids;
            openids.push_back(SdkTestDataCreateUtils::GetOpenidHashTailStrategy(std::vector<std::string>{"00"}));
            
            strategy_groups.push_back(imei_group);
            strategy_groups.push_back(app_client);
            strategy_groups.push_back(openids);
            
            plan.group_list = strategy_groups;
        } else if (i == 1) {
            std::vector<std::vector<Strategy>> strategy_groups;
    
            //设备标志符尾号为00，hash尾号也为00
            std::vector<Strategy> imei_group;
            imei_group.push_back(SdkTestDataCreateUtils::GetDeviceUidTailStrategy(std::vector<std::string>{"0"}));
            imei_group.push_back(SdkTestDataCreateUtils::GetDeviceUidHashStrategy(std::vector<std::string>{"00"}));
            //指定客户端策略
            std::vector<Strategy> app_client;
            app_client.push_back(SdkTestDataCreateUtils::GetForbidAppClientStrategy());
            //openid hash尾号为00
            std::vector<Strategy> openids;
            openids.push_back(SdkTestDataCreateUtils::GetOpenidHashTailStrategy(std::vector<std::string>{"00"}));
    
            strategy_groups.push_back(imei_group);
            strategy_groups.push_back(app_client);
            strategy_groups.push_back(openids);
    
            plan.group_list = strategy_groups;
        }
        
        plan.params = params;
        plans.push_back(plan);
    }
    test->plans = plans;
}

TEST_F(GrayTestQueryClientTest2, StringToSet_Test) {
    std::string str = "00";
    std::set<std::string> str_set;
    
    SplitStringToSet(str, "|", &str_set);
    
    int len = str_set.size();
    int count = str_set.count("00");
    
    EXPECT_EQ(1, len);
    EXPECT_EQ(1, count);
}

TEST_F(GrayTestQueryClientTest2, StringToSet2_Test) {
    std::string str = "00|01|03";
    std::set<std::string> str_set;
    
    SplitStringToSet(str, "|", &str_set);

    int len = str_set.size();
    int count0 = str_set.count("00");
    int count1 = str_set.count("01");
    int count3 = str_set.count("03");

    EXPECT_EQ(3, len);
    EXPECT_EQ(1, count0);
    EXPECT_EQ(1, count1);
    EXPECT_EQ(1, count3);
}

TEST_F(GrayTestQueryClientTest2, StringToVector_Test) {
    std::string str = "00";
    std::vector<std::string> str_vec;
    
    SplitString(str, "|", &str_vec);

    int len = str_vec.size();

    EXPECT_EQ(1, len);
    EXPECT_EQ("00", str_vec[0]);
}

TEST_F(GrayTestQueryClientTest2, StringToVector2_Test) {
    std::string str = "00|01|03";
    std::vector<std::string> str_vec;
    
    SplitString(str, "|", &str_vec);

    int len = str_vec.size();
    
    EXPECT_EQ(3, len);
    EXPECT_EQ("00", str_vec[0]);
    EXPECT_EQ("01", str_vec[1]);
    EXPECT_EQ("03", str_vec[2]);
}

TEST_F(GrayTestQueryClientTest2, QueryTestPlan_whiteList_Test) {
    std::map<std::string, std::string> filter;
    filter.insert(std::make_pair("imei", "whiteList1"));
    
    std::shared_ptr<TestPlanQuery> query = client->QueryTestPlan("moduleCode", "testCode", filter);
    EXPECT_EQ("2", query->paramList[0].paramValue);
    
    filter["imei"] = "whiteList2";
    query = client->QueryTestPlan("moduleCode", "testCode", filter);
    EXPECT_EQ("1", query->paramList[0].paramValue);
    
    filter["imei"] = "whiteList3";
    query = client->QueryTestPlan("moduleCode", "testCode", filter);
    EXPECT_EQ("0", query->paramList[0].paramValue);
}

TEST_F(GrayTestQueryClientTest2, QueryTestPlan_matchPlan1_Test) {
    std::string imei00;
    while (true) {
        imei00 = RandomStringUtils::Generate(10) + "00";
        if (HashCodeUtils::GetHashOnePercent("testKey" + imei00, "MD5") == "00") {
            break;
        }
    }
    
    std::cout << "imei00: " << imei00 << endl;
    
    std::map<std::string, std::string> filter;
    filter.insert(std::make_pair("imei", imei00));
    filter.insert(std::make_pair("openid", imei00));
    filter.insert(std::make_pair("packageName", "appoint1"));
    filter.insert(std::make_pair("version", "1.0"));
    
    std::shared_ptr<TestPlanQuery> query = client->QueryTestPlan("moduleCode", "testCode", filter);
    EXPECT_EQ("2", query->paramList[0].paramValue);
}

TEST_F(GrayTestQueryClientTest2, QueryTestPlan_rejectPlan2_Test) {
    std::string imei00;
    while (true) {
        imei00 = RandomStringUtils::Generate(10) + "00";
        if (HashCodeUtils::GetHashOnePercent("testKey" + imei00, "MD5") == "00") {
            break;
        }
    }
    
    std::map<std::string, std::string> filter;
    filter.insert(std::make_pair("imei", imei00));
    filter.insert(std::make_pair("openid", imei00));
    filter.insert(std::make_pair("packageName", "forbid1"));
    filter.insert(std::make_pair("version", "1.0"));
    
    std::shared_ptr<TestPlanQuery> query = client->QueryTestPlan("moduleCode", "testCode", filter);
    EXPECT_EQ("0", query->paramList[0].paramValue);
}

TEST_F(GrayTestQueryClientTest2, QueryTestPlan_matchPlan2_Test) {
    std::string imei00;
    while (true) {
        imei00 = RandomStringUtils::Generate(10) + "00";
        if (HashCodeUtils::GetHashOnePercent("testKey" + imei00, "MD5") == "00") {
            break;
        }
    }
    
    std::map<std::string, std::string> filter;
    filter.insert(std::make_pair("imei", imei00));
    filter.insert(std::make_pair("openid", imei00));
    filter.insert(std::make_pair("packageName", "forbid1"));
    filter.insert(std::make_pair("version", "0.0"));
    
    std::shared_ptr<TestPlanQuery> query = client->QueryTestPlan("moduleCode", "testCode", filter);
    EXPECT_EQ("1", query->paramList[0].paramValue);
}

TEST_F(GrayTestQueryClientTest2, QueryTestPlan_matchDefaultPlan_Test) {
    std::string imei00 = "test00";
    
    std::map<std::string, std::string> filter;
    filter.insert(std::make_pair("imei", imei00));
    filter.insert(std::make_pair("openid", imei00));
    filter.insert(std::make_pair("packageName", "forbid1"));
    filter.insert(std::make_pair("version", "0.0"));
    
    std::shared_ptr<TestPlanQuery> query = client->QueryTestPlan("moduleCode", "testCode", filter);
    EXPECT_EQ("0", query->paramList[0].paramValue);
}

int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}
