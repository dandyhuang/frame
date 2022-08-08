//
// Created by 吴婷 on 2020-07-01.
//

#ifndef COMMON_LIBVNS_NAMING_SERVICE_H
#define COMMON_LIBVNS_NAMING_SERVICE_H

#include <map>
#include <vector>
#include <string>
#include <memory>

#include "core/host_reactor.h"
#include "beat/beat_reactor.h"
#include "naming_proxy.h"

namespace common {
/**
 * 名字服务类
 */
class NamingService {
public:
    NamingService();
    ~NamingService();
    explicit NamingService(std::map<std::string, std::string> properties);

    /**
     * 启动连接名字服务
     */
    bool init();

    /**
     * 注册实例
     * @param service_name 服务名
     * @param group_name 分组
     * @param instance 实例
     */
    bool registerInstance(std::string service_name, std::string group_name, Instance instance);

    bool registerInstance(std::string service_name, std::string ip, int port);

    /**
     * 注册
     * @param service_name 服务名
     * @param group_name 组名
     * @param ip
     * @param port
     * @param cluster_name 集群名
     */
    bool registerInstance(std::string service_name, std::string group_name, std::string ip, int port, std::string cluster_name);

    /**
     * 注销实例
     * @param service_name
     * @param group_name
     * @param instance
     */
    void deregisterInstance(std::string service_name, std::string group_name, Instance instance);

    /**
     * 查询/订阅实例信息
     * @param service_name 服务名
     * @param group_name 组名
     * @param clusters 集群名
     * @param subscribe 是否订阅
     * @return 符合条件的所有实例
     */
    std::vector<Instance> getAllInstances(std::string service_name, std::string group_name, std::vector<std::string> clusters, bool subscribe);

    /**
     * 查询/订阅健康实例
     * @param service_name
     * @param group_name
     * @param clusters
     * @param healthy 是否只返回健康实例
     * @param subscribe
     * @return 符合条件的所有实例
     */
    std::vector<Instance> selectInstances(std::string service_name, std::string group_name,
            std::vector<std::string> clusters, bool healthy, bool subscribe);

    std::vector<Instance> selectInstances(std::string service_name, bool healthy, bool subscribe);

    /**
     * 设置接收推送的UDP端口，默认为1111
     */
    void setUDPPort(int port);

private:
    /**
     * 选取服务健康的实例
     * @param service
     * @param healthy
     * @return 返回的实例
     */
    std::vector<Instance> selectInstances(std::shared_ptr<ServiceInfo> service, bool healthy);

private:
    // 从配置中获取应用名
    void initAppName(std::map<std::string, std::string> properties);
    // 从配置中获取空间名
    void initNamespace(std::map<std::string, std::string> properties);
    // 从配置中获取集群名
    void initClusterName(std::map<std::string, std::string> properties);
    // 从配置中获取机房环境
    void initAppLoc(std::map<std::string, std::string> properties);
    // 初始化缓存地址
    void initCacheDir(std::map<std::string, std::string> properties);
    // 从配置中获取心跳线程数
    int initClientBeatThreadCount(std::map<std::string, std::string> properties);
    // 从配置中获取HostReactor线程数
    int initPollingThreadCount(std::map<std::string, std::string> properties);
    // 从配置中获取是否从缓存中加载
    bool isLoadCacheAtStart(std::map<std::string, std::string> properties);

private:
    static bool _init_connect; // 初始化是否成功
    std::string _namespace;
    std::string _cluster_name;  // 集群名
    std::string _cache_dir;
    std::shared_ptr<NamingProxy> _server_proxy;     // 与agent具体通信
    BeatReactor* _beat_reactor = NULL;              // 心跳上报
    std::shared_ptr<HostReactor> _host_reactor;     // 用于获取、保存、更新各service实例信息

    std::map<std::string, std::string> _properties; // 传入配置参数

};

}

#endif //COMMON_LIBVNS_NAMING_SERVICE_H
