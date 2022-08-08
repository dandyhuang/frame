//
// Created by 吴婷 on 2020-06-16.
//

#ifndef COMMON_LIBVNS_INSTANCE_H
#define COMMON_LIBVNS_INSTANCE_H

#include <string>
#include <map>

/**
 * 实例
 */
class Instance {
public:
    std::string toString();
    std::string toInetAddr();
    long getInstanceHeartBeatInterval();    // 获取心跳上报间隔：5秒
    long getInstanceHeartBeatTimeOut();     // 获取心跳超时时间：15秒
    long getIpDeleteTimeout();              // ip删除超时时间：30秒
    std::string getInstanceIdGenerator();

private:
    /**
     * 从matadata中取值，无就返回默认值
     * @param key
     * @param default_value 默认值
     * @return
     */
    long getMetaDataByKeyWithDefault(std::string key, long default_value);

    std::string getMetaDataByKeyWithDefault(std::string key, std::string default_value);

public:
    std::string instance_id;
    std::string ip;             // ip
    int port;                   // 端口
    double weight = 1;          // 权重
    bool healthy = true;        // 健康状态
    bool enabled = true;        // 是否可用: true-上线，false-下线
    bool ephemeral = true;      // 是否为临时节点
    std::string cluster_name;   // 集群名
    std::string service_name;   // 服务名

    std::map<std::string, std::string> metadata;    // 扩展属性
};


#endif //COMMON_LIBVNS_INSTANCE_H
