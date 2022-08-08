//
// Created by 吴婷 on 2020-06-17.
//

#ifndef COMMON_LIBVNS_FAILOVER_REACTOR_H
#define COMMON_LIBVNS_FAILOVER_REACTOR_H

#include <memory>
#include <unordered_map>
#include "thirdparty/monitor_sdk_common/system/threading/mutex.h"
//#include "core/host_reactor.h"
#include "object/service_info.h"
#include "scheduled/scheduled_executor.h"

namespace common {
class HostReactor;  // 相互引用

/**
 * failover模式 故障转移
 * 一个线程定时读取
 * 1.开关文件读取
 * 2.缓存定时更新
 */
class FailoverReactor {
public:
    FailoverReactor(HostReactor* host_reactor, const std::string &cache_dir)
    : host_reactor(host_reactor), _failover_dir(cache_dir + "/failover") {}

    ~FailoverReactor();

    void init();

    /**
     * 判断是否开启failover模式
     * @return 设置状态
     */
    bool isFailoverSwitch();

    /**
     * 获取服务信息
     * @param key 服务key
     * @return 返回具体服务
     */
    std::shared_ptr<ServiceInfo> getService(const std::string &key);

    std::string getFailoverDir();

public:
    std::unordered_map<std::string, std::shared_ptr<ServiceInfo>> service_map;  // 容灾模式的缓存
    std::unordered_map<std::string, bool> switch_params;                        // 开关设置
    HostReactor* host_reactor;

private:
    std::string _failover_dir;    // 故障转移路径
    std::shared_ptr<ScheduledExecutor> _scheduled_executor = nullptr; // 调度类：定时更新容灾信息
    static int DAY_PERIOD_MINUTES;

};

/**
 * 刷新落盘服务文件
 */
class SwitchRefresher {
public:
    explicit SwitchRefresher(FailoverReactor* failover_reactor);
    void run();

private:
    FailoverReactor* _failover_reactor;
    std::string _failover_dir;
    long _last_modified_millis = 0;
    Mutex _mutex;

};

/**
 * 读取缓存服务文件
 */
class FailoverFileReader {
public:
    explicit FailoverFileReader(FailoverReactor* failover_reactor);
    void run();

private:
    FailoverReactor* _failover_reactor;
    std::string _failover_dir;

};

/**
 * 数据落盘
 */
class DiskFileWriter {
public:
    explicit DiskFileWriter(FailoverReactor* failover_reactor);
    void run();

private:
    FailoverReactor* _failover_reactor;
    std::string _failover_dir;

};

}   // namespace common

#endif //COMMON_LIBVNS_FAILOVER_REACTOR_H
