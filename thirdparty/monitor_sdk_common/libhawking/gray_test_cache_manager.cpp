//
// Created by 陈嘉豪 on 2019/12/4.
//

#include "thirdparty/monitor_sdk_common/libhawking/gray_test_cache_manager.h"

using namespace common;

// 刷新缓存
void GrayTestCacheManager::Refresh(std::shared_ptr<Business> business,
        std::vector<std::shared_ptr<Module>> modules) {
    GrayTestCache::SetBusiness(business);
    
    std::map<std::string, std::shared_ptr<Module>> module_map;
    for (auto module : modules) {
        module_map.insert(std::make_pair(module->code, module));
    }
    GrayTestCache::SetModules(module_map);
}

// 检索实验配置
std::shared_ptr<TestQueryResult> GrayTestCacheManager::GetGrayTest(const std::string& module_code,
        const std::string& test_code) {
    std::shared_ptr<Module> module = GrayTestCache::QueryModule(module_code);
    if (module == nullptr) {
        return std::shared_ptr<TestQueryResult>(new TestQueryResult(RetCode::MODULE_CODE_ERROR));
    }
    
    std::shared_ptr<GrayTest> new_gray_test;
    for (auto gray_test : module->tests) {
        if (gray_test.code == test_code) {
            new_gray_test = std::make_shared<GrayTest>(gray_test);
        }
    }
    if (new_gray_test == nullptr) {
        return std::shared_ptr<TestQueryResult>(new TestQueryResult(RetCode::TEST_CODE_ERROR));
    }
    
    // 校验实验时间
    if (new_gray_test->eff_type == GrayTestConfig::test_eff_type.TIMING) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        long now = tv.tv_sec;
        if (now < new_gray_test->eff_time || now > new_gray_test->exp_time) {
            // 未生效 或 已过期
            return std::shared_ptr<TestQueryResult>(new TestQueryResult(RetCode::TEST_STATUS_ERROR));
        }
    }
    
    return std::shared_ptr<TestQueryResult>(new TestQueryResult(new_gray_test));
}
