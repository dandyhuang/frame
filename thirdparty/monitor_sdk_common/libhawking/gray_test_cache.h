//
// Created by 陈嘉豪 on 2019/12/4.
//

#ifndef COMMON_LIBHAWKING_GRAY_TEST_CACHE_H
#define COMMON_LIBHAWKING_GRAY_TEST_CACHE_H

#include <map>
#include <memory>
#include <string>

#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "thirdparty/monitor_sdk_common/libhawking/item.h"

namespace common {

class GrayTestCache : public SingletonBase<GrayTestCache> {
public:
    static std::shared_ptr<Business> GetBusiness() {
        return Instance()->business_;
    }
    
    static void SetBusiness(std::shared_ptr<Business> business) {
        Instance()->business_ = business;
    }
    
    static std::shared_ptr<Module> QueryModule(const std::string& module_code) {
        return Instance()->modules_[module_code];
    }
    
    static std::map<std::string, std::shared_ptr<Module>> GetModules() {
        return Instance()->modules_;
    }
    
    static void SetModules(std::map<std::string, std::shared_ptr<Module>> modules) {
        Instance()->modules_ = modules;
    }
    
private:
    std::shared_ptr<Business> business_;
    std::map<std::string, std::shared_ptr<Module>> modules_;
};

} // namespace common

#endif //COMMON_LIBHAWKING_GRAY_TEST_CACHE_H
