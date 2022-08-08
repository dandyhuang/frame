//
// Created by 陈嘉豪 on 2019/12/5.
//

#ifndef COMMON_LIBHAWKING_DEVICE_UID_PROVIDER_H
#define COMMON_LIBHAWKING_DEVICE_UID_PROVIDER_H

#include <map>
#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/libhawking/gray_test_config.h"

namespace common {

class DeviceUidProvider {
public:
    static std::vector<std::string> uid_order;
    
    static bool uid_order_init;
    
    static std::string GetDeviceUid(const std::map<std::string, std::string>& filter_param);
    
    static void InitUidOrder();
};

} // namespace common

#endif //COMMON_LIBHAWKING_DEVICE_UID_PROVIDER_H
