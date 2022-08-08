//
// Created by 吴婷 on 2020-06-16.
//
#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "object/instance.h"
#include "utils/json_utils.h"
#include "constants.h"

using namespace common;

std::string Instance::toString() {
    return JsonUtils::ToJSONString(*this);
}

std::string Instance::toInetAddr() {
    return ip + ":" + IntegerToString(port);
}

long Instance::getInstanceHeartBeatInterval() {
    return getMetaDataByKeyWithDefault("preserved.heart.beat.interval", Constants::DEFAULT_HEART_BEAT_INTERVAL);
}

long Instance::getInstanceHeartBeatTimeOut() {
    return getMetaDataByKeyWithDefault("preserved.heart.beat.timeout", Constants::DEFAULT_HEART_BEAT_TIMEOUT);
}

long Instance::getIpDeleteTimeout() {
    return getMetaDataByKeyWithDefault("preserved.ip.delete.timeout", Constants::DEFAULT_IP_DELETE_TIMEOUT);
}

std::string Instance::getInstanceIdGenerator() {
    return getMetaDataByKeyWithDefault("preserved.instance.id.generator", "simple");
}

long Instance::getMetaDataByKeyWithDefault(std::string key, long default_value){
    if(metadata.size() == 0){
        return default_value;
    }
    auto iter = metadata.find(key);
    if (iter != metadata.end()) {  //包含
        long value;
        StringToNumber(iter->second, &value);
        return value;
    }
    return default_value;
}

std::string Instance::getMetaDataByKeyWithDefault(std::string key, std::string default_value){
    if(metadata.size() == 0){
        return default_value;
    }
    auto iter = metadata.find(key);
    if (iter != metadata.end()) {  //包含
        return iter->second;
    }
    return default_value;
}