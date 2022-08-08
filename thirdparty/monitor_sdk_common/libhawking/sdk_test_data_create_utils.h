//
// Created by 陈嘉豪 on 2019/12/16.
//

#ifndef COMMON_LIBHAWKING_SDK_TEST_DATA_CREATE_UTILS_H
#define COMMON_LIBHAWKING_SDK_TEST_DATA_CREATE_UTILS_H

#include <string>

#include "thirdparty/monitor_sdk_common/libhawking/id_utils.h"
#include "thirdparty/monitor_sdk_common/libhawking/item.h"

namespace common {

class SdkTestDataCreateUtils {
public:
    static Strategy GetForbidAppClientStrategy() {
        Strategy strategy;
        strategy.content = "1";
        strategy.id = IdUtils::GetNextId();
        strategy.type = 1;
        
        std::vector<ClientStrategy> client_strategies;
        ClientStrategy client_strategy;
        client_strategy.package_name = "forbid1";
        client_strategy.version = "1.0";
        client_strategies.push_back(client_strategy);
        
        ClientStrategy client_strategy2;
        client_strategy2.package_name = "forbid2";
        client_strategy2.version = "2.0";
        client_strategies.push_back(client_strategy2);
        
        strategy.client_strategies = client_strategies;
        return strategy;
    }
    
    static Strategy GetAppointAppClientStrategy() {
        Strategy strategy;
        strategy.content = "0";
        strategy.id = IdUtils::GetNextId();
        strategy.type = 1;
        
        std::vector<ClientStrategy> client_strategies;
        ClientStrategy client_strategy;
        client_strategy.package_name = "appoint1";
        client_strategy.version = "1.0";
        client_strategies.push_back(client_strategy);
        
        ClientStrategy client_strategy2;
        client_strategy2.package_name = "appoint2";
        client_strategy2.version = "2.0";
        client_strategies.push_back(client_strategy2);
        
        strategy.client_strategies = client_strategies;
        return strategy;
    }
    
    static Strategy GetAppVersionStrategy(const std::vector<std::string>& versions) {
        Strategy strategy;
        strategy.id = IdUtils::GetNextId();
        strategy.type = 2;
        std::string content;
        for (size_t i = 0; i < versions.size(); ++i) {
            content += versions[i];
            if (i < versions.size() - 1) {
                content += "|";
            }
        }
        strategy.content = content;
        return strategy;
    }
    
    static Strategy GetDeviceUidHashStrategy(const std::vector<std::string>& hash_tails) {
        Strategy strategy;
        strategy.id = IdUtils::GetNextId();
        strategy.type = 5;
        std::string content;
        for (size_t i = 0; i < hash_tails.size(); ++i) {
            content += hash_tails[i];
            if (i < hash_tails.size() - 1) {
                content += "|";
            }
        }
        strategy.content = content;
        return strategy;
    }
    
    static Strategy GetDeviceUidTailStrategy(const std::vector<std::string>& tails) {
        Strategy strategy;
        strategy.id = IdUtils::GetNextId();
        strategy.type = 4;
        std::string content;
        for (size_t i = 0; i < tails.size(); ++i) {
            content += tails[i];
            if (i < tails.size() - 1) {
                content += "|";
            }
        }
        strategy.content = content;
        return strategy;
    }
    
    static Strategy GetOpenidHashTailStrategy(const std::vector<std::string>& tails) {
        Strategy strategy;
        strategy.id = IdUtils::GetNextId();
        strategy.type = 6;
        std::string content;
        for (size_t i = 0; i < tails.size(); ++i) {
            content += tails[i];
            if (i < tails.size() - 1) {
                content += "|";
            }
        }
        strategy.content = content;
        return strategy;
    }
};

} //namespace common

#endif //COMMON_LIBHAWKING_SDK_TEST_DATA_CREATE_UTILS_H
