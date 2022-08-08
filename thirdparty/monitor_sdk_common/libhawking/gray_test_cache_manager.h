//
// Created by 陈嘉豪 on 2019/12/4.
//

#ifndef COMMON_LIBHAWKING_GRAY_TEST_CACHE_MANAGER_H
#define COMMON_LIBHAWKING_GRAY_TEST_CACHE_MANAGER_H

#include <map>
#include <memory>
#include <string>
#include <sys/time.h>
#include <vector>

#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_cache.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_config.h"
#include "thirdparty/monitor_sdk_common/libhawking/item.h"

namespace common {

class GrayTestCacheManager {
public:
    GrayTestCacheManager() = default;
    
    ~GrayTestCacheManager() = default;
    
    static void Refresh(std::shared_ptr<Business> business, std::vector<std::shared_ptr<Module>> modules);
    
    static std::shared_ptr<TestQueryResult> GetGrayTest(const std::string& module_code, const std::string& test_code);
};

} // namespace common

#endif //COMMON_LIBHAWKING_GRAY_TEST_CACHE_MANAGER_H
