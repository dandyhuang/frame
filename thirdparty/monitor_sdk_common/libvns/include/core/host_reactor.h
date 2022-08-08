//
// Created by 吴婷 on 2020-06-16.
//

#ifndef COMMON_LIBVNS_HOST_REACTOR_H
#define COMMON_LIBVNS_HOST_REACTOR_H

#include <climits>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include "naming_proxy.h"
#include "object/service_info.h"
#include "beat/beat_reactor.h"
#include "scheduled/scheduled_executor.h"
#include "backups/failover_reactor.h"
#include "core/push_receiver.h"
#include "utils/utilandcoms.h"

namespace common {
//class FailoverReactor;

/**
 * 用于获取、保存、更新各service实例信息
 */
class HostReactor {
public:
    HostReactor(std::shared_ptr<NamingProxy> server_proxy,
                const std::string &cache_dir, bool load_cache_at_start = false,
                int polling_thread_count = UtilAndComs::DEFAULT_POLLING_THREAD_COUNT)
            : _server_proxy(server_proxy)
            , _cache_dir(cache_dir)
            , _load_cache_at_start(load_cache_at_start)
            , _polling_thread_count(polling_thread_count)
            {};

    void initialize();

    /**
     * 获取服务信息（订阅）
     * @param service_name 服务名
     * @param clusters 集群名
     * @return 返回的服务信息
     */
    std::shared_ptr<ServiceInfo> getServiceInfo(const std::string &service_name, const std::string &clusters);

    /**
     * 获取服务信息（查询）
     * @param service_name
     * @param clusters
     * @return
     */
    std::shared_ptr<ServiceInfo> getServiceInfoDirectlyFromServer(const std::string &service_name, const std::string &clusters);

    /**
     * 解析API返回的json信息
     * @param json
     * @return
     */
    std::shared_ptr<ServiceInfo> processServiceJSON(const std::string &json);

    /**
     * pull主动拉取更新服务
     * @param service_name
     * @param clusters
     */
    void updateServiceNow(const std::string &service_name, const std::string &clusters);

    /**
     * 仅刷新，不更新本地服务
     * @param service_name
     * @param clusters
     */
    void refreshOnly(const std::string &service_name, const std::string &clusters);

public:
    /**
     * 保存已获取服务的信息：{服务名@@集群名, 服务信息}
     */
    std::unordered_map<std::string, std::shared_ptr<ServiceInfo>> serviceInfo_map;

    /**
     * 保存待更新服务key
     */
    std::unordered_map<std::string, std::shared_ptr<ServiceInfo>> updating_map;


private:
    static int DEFAULT_DELAY;           // 默认更新任务轮询间隔（单位：毫秒）
    static int UPDATE_HOLD_INTERVAL;    // 默认更新保持时间（单位：毫秒）
    std::shared_ptr<NamingProxy> _server_proxy;
    //std::shared_ptr<BeatReactor> _beat_reactor;    // 心跳上报
    std::shared_ptr<ScheduledExecutor> _scheduled_executor = nullptr; // 调度类：定时更新服务信息
    std::shared_ptr<FailoverReactor> _failover_reactor;    // failover模式 故障转移
    std::shared_ptr<PushReceiver> _push_receiver;          // 接受服务端的UDP推送
    //EventDispatcher _eventDispatcher;
    std::string _cache_dir;         // 缓存路径
    bool _load_cache_at_start;
    int _polling_thread_count;      // 线程数

};

/**
 * 服务定时更新任务
 */
class UpdateTask {
public:
    UpdateTask(const std::string &service_name, const std::string &clusters, HostReactor* host_reactor)
    : _service_name(service_name), _clusters(clusters), _host_reactor(host_reactor) {};

    ~UpdateTask(){};

    void run();     // 定时拉取更新

private:
    std::string _service_name;      // 服务名
    std::string _clusters;          // 集群名
    long _last_ref_time = LONG_MAX;  // 最近刷新时间

    HostReactor* _host_reactor;
};

}  //namespace common

#endif //COMMON_LIBVNS_HOST_REACTOR_H
