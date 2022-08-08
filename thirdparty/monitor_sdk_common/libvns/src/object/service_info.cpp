//
// Created by 吴婷 on 2020-06-16.
//

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "object/service_info.h"
#include "utils/utils.h"
#include "utils/json_utils.h"

using namespace common;

std::string ServiceInfo::ALL_IPS = "000--00-ALL_IPS--00--000";

ServiceInfo::ServiceInfo(std::string key){
    int max_index = 2;
    int cluster_index = 1;
    int service_name_index = 0;

    std::vector<std::string> keys;
    SplitString(key, "@@", &keys);    //按:分隔string
    int len = keys.size();
    if (len >= max_index) {
        name = keys[service_name_index];
        clusters = keys[cluster_index];
    }
    name = keys[0];
}

int ServiceInfo::ipCount() {
    return hosts.size();
}
bool ServiceInfo::expired() {
    return (VnsTimeUtils::GetCurrentTimeMills() - last_ref_time > cache_millis) ? true : false;
}
bool ServiceInfo::isValid() {
    return hosts.size()>0 ? true : false;
}
bool ServiceInfo::validate() {
    // TODO 待实现
    return true;
}

std::string ServiceInfo::getServiceName(){
    // 如果名字前有默认分组，则删除
    auto it = name.find("@@");
    if(it != std::string::npos){
        std::string service = name.substr(it+2);
        return service;
    }
    return name;
}

std::string ServiceInfo::getKey(){
    return getKey(name, clusters);
}

std::string ServiceInfo::getKey(std::string name, std::string clusters) {
    return clusters.empty() ? name : (name + "@@" + clusters);
}

std::string ServiceInfo::toString(){
    return JsonUtils::ToJSONString(*this);
}
