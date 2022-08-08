//
// Created by 吴婷 on 2020-06-15.
//

#ifndef LIBVNS_BEATREACTOR_H
#define LIBVNS_BEATREACTOR_H

#include <memory>
#include <unordered_map>

#include "thirdparty/monitor_sdk_common/system/threading/thread.h"
#include "thirdparty/monitor_sdk_common/system/threading/thread_pool.h"
#include "thirdparty/monitor_sdk_common/system/threading/mutex.h"
#include "naming_proxy.h"
#include "beat/beat_info.h"
#include "scheduled/scheduled_executor.h"
#include "utils/utilandcoms.h"

namespace common {

class BeatTask;

/**
 * 向agent发送已注册服务的心跳
 */
class BeatReactor {
public:
    BeatReactor(std::shared_ptr<NamingProxy> server_proxy, int thread_count = UtilAndComs::DEFAULT_CLIENT_BEAT_THREAD_COUNT)
    : _server_proxy(server_proxy), _thread_count(thread_count) {
        _thread_pool = std::make_shared<ThreadPool>(_thread_count);      // 线程池
        _beatMaster = new Thread(); // 管理所有心跳任务的线程
    }

    ~BeatReactor();

    /**
     * 初始化
     */
    void initialize();

    /**
     * 增加心跳上报任务
     * @param service_name 服务名
     * @param beatInfo 上报信息
     */
    void addBeatInfo(const std::string &service_name, BeatInfo &beatInfo);

    /**
     * 删除上报任务
     * @param service_name 服务名
     * @param ip
     * @param port
     */
    void removeBeatInfo(const std::string &service_name, const std::string &ip, int port);

    /**
     * 重置上报时间间隔
     * @param interval
     */
    void setClientBeatInterval(int interval);

    /**
     * 构建key
     * @param service_name 服务名
     * @param ip
     * @param port
     * @return key
     */
    std::string buildKey(const std::string &service_name, const std::string &ip, int port);

public:
    /**
     * 定时处理心跳任务列表
     * @param param
     * @return
     */
    static void *beatMater(void *param);

private:
    volatile bool _stop;
    std::shared_ptr<NamingProxy> _server_proxy;   //与agent具体通信
    std::shared_ptr<ThreadPool> _thread_pool = nullptr;               // 线程池
    Thread *_beatMaster;                            // 心跳处理线程
    int _thread_count;
    Mutex _mutex;
    int _client_beatinterval = 5000;        // 上报时间间隔（毫秒），默认5秒上报一次
    std::unordered_map<std::string, std::shared_ptr<BeatTask>> beatinfo_list;  //任务列表: {server#ip#port, beattask}
};

/**
 * 心跳上报任务
 */
class BeatTask {
public:
    BeatTask(BeatInfo &beatInfo, std::shared_ptr<NamingProxy> server_proxy,
             BeatReactor* beat_reactor);

    ~BeatTask(){};

    void run();     // 上报心跳

public:
    BeatInfo beatInfo;

private:
    std::shared_ptr<NamingProxy> _server_proxy;   //与agent具体通信
    BeatReactor* _beat_reactor;
};

}  //namespace common

#endif //LIBVNS_BEATREACTOR_H
