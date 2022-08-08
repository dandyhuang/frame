//
// Created by 陈嘉豪 on 2019/12/5.
//

#include "thirdparty/monitor_sdk_common/libhawking/device_uid_provider.h"

using namespace common;

bool DeviceUidProvider::uid_order_init = false;

std::vector<std::string> DeviceUidProvider::uid_order{};

std::string DeviceUidProvider::GetDeviceUid(const std::map<std::string, std::string> &filter_param) {
    if (!uid_order_init) {
        InitUidOrder();
    }
    if (filter_param.empty()) {
        return "";
    }
    std::string uid;
    for (const std::string& uid_type : uid_order) {
        if (filter_param.count(uid_type)) {
            uid = filter_param.at(uid_type);
        }
        if (!uid.empty()) {
            return uid;
        }
    }
    return "";
}

void DeviceUidProvider::InitUidOrder() {
    uid_order = std::vector<std::string>{
            GrayTestConfig::filter_key.IMEI, GrayTestConfig::filter_key.VAID,
            GrayTestConfig::filter_key.OAID, GrayTestConfig::filter_key.AAID
    };
    uid_order_init = true;
}
