//
// Created by 吴婷 on 2020-07-01.
//

#ifndef COMMON_LIBVNS_COMMON_PARAMS_H
#define COMMON_LIBVNS_COMMON_PARAMS_H

#include <string>

/**
 * API请求参数
 */
class CommonParams {
public:
    static const std::string APP;

    static const std::string TAG;

    static const std::string PID;

    static const std::string CODE;

    static const std::string SERVICE_NAME;

    static const std::string CLUSTER_NAME;

    static const std::string NAMESPACE_ID;

    static const std::string GROUP_NAME;

    static const std::string ACCESS_TOKEN;

    static const std::string APP_ENV;
};

/**
 * 配置key
 */
class PropertyKeyConst
{
public:
    static int UDP_PORT;

    static const std::string CACHE_PATH;

    static const std::string IS_USE_ENDPOINT_PARSING_RULE;

    static const std::string ENDPOINT;

    static const std::string ENDPOINT_PORT;

    static const std::string NAMESPACE;

    static const std::string APP_ENV;

    static const std::string APP_SECRET;

    static const std::string RAM_ROLE_NAME;

    static const std::string SERVER_ADDR;

    static const std::string CONTEXT_PATH;

    static const std::string APP_NAME;   // 必填：CMDB节点上的应用名

    static const std::string APP_LOC;

    static const std::string CLUSTER_NAME;   // 必填：集群名

    static const std::string ENCODE;

    static const std::string NAMING_LOAD_CACHE_AT_START;

    static const std::string NAMING_CLIENT_BEAT_THREAD_COUNT;

    static const std::string NAMING_POLLING_THREAD_COUNT;

};

class LocalProperties{
public:
    static std::string app_name;    // 应用名
    static std::string tag;         // tag
    static std::string ip;          // 本机ip
    static std::string pid;         // 当前进程号
    static std::string env;         // 环境
    static std::string app_loc;         // 机房环境: 手动填写时需要
};

#endif //COMMON_LIBVNS_COMMON_PARAMS_H
