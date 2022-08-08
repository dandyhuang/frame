//
// Created by 吴婷 on 2020-06-16.
//

#ifndef COMMON_LIBVNS_SERVICE_INFO_H
#define COMMON_LIBVNS_SERVICE_INFO_H

#include <string>
#include <vector>
#include "object/instance.h"

/**
 * 服务信息
 */
class ServiceInfo {
public:
    ServiceInfo(){};
    ServiceInfo(std::string key);
    ServiceInfo(std::string name, std::string clusters): name(name), clusters(clusters){};

    int ipCount();  // ip数
    bool expired(); // 是否过期
    bool isValid(); // 是否合法
    bool validate();// 是否有效
    std::string getKey();
    std::string getServiceName(); // 仅服务名

    static std::string getKey(std::string name, std::string clusters);

    std::string toString();

public:
    std::string json_from_server;
    std::string name;       // 服务名
    std::string group_name;
    std::string clusters;   // 集群名
    long cache_millis = 1000L;
    std::vector<Instance> hosts;
    long last_ref_time = 0;
    std::string checksum;
    bool all_ips = false;

    static std::string ALL_IPS;

};


#endif //COMMON_LIBVNS_SERVICE_INFO_H
