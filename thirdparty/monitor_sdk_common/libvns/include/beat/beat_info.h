//
// Created by 吴婷 on 2020-06-15.
//

#ifndef LIBVNS_BEATINFO_H
#define LIBVNS_BEATINFO_H

#include <map>
#include <string>

/**
 * 心跳上报信息
 */
class BeatInfo {
public:
    std::string toString();

public:
    int port;                   // 端口
    std::string ip;             // ip
    double weight;              // 权重
    std::string service_name;   // 服务名
    std::string cluster_name;   // 集群名
    std::map<std::string, std::string> metadata;    // 其他信息
    bool scheduled;             // 是否心跳中

    long period = 5000;        // 默认5秒，单位：毫秒
    bool stopped = false;               // 是否停止上报
};

/**
 * 心跳返回结果
 */
class BeatResult {
public:
    int client_beat_interval = -1;   // 心跳间隔
};


#endif //LIBVNS_BEATINFO_H
