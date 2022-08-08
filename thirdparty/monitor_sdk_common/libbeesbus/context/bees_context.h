//
// Created by 杨民善 on 2020/6/16.
//

#ifndef BLADE_BASE_COMMON_BEES_CONTEXT_H
#define BLADE_BASE_COMMON_BEES_CONTEXT_H

#include "../util/util.h"
#include "../client/thrift_client.h"
#include "../log/glog_helper.h"

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <set>
#include <vector>
#include <utility>
#include <unordered_map>

#include "thirdparty/monitor_sdk_common/net/http/client.h"

namespace vivo_bees_bus_cpp_report
{
    struct Report2BeesInfo
    {
        std::string localIp;                              // 本机 ip，用于从 cmdb 获取机房信息
        std::string beesManagerUrl;                       // bees-manager 接口地址，用于获取 bees-bus 主机列表和上报心跳 - 60秒上报一次
        int sendTimeout{DEFAULT_THRIFT_TIMEOUT};          // 发送超时时间
        int recvTimeout{DEFAULT_THRIFT_TIMEOUT};          // 接收超时时间
        int retryMax{DEFAULT_THRIFT_RETRY_MAX};           // 最大重试次数
        int beesBusPort{BEES_THRIFT_PORT};                // bees bus 上报端口
    };

    struct AppendContext
    {
        std::string taskName;

        AppendContext() = default;
        explicit AppendContext(std::string task) : taskName(std::move(task)) {}
    };

    struct ThriftClientInfo
    {
        ThriftClient *pThriftClient{nullptr};
        std::mutex *pThriftMutex{nullptr};

        ThriftClientInfo() = default;
        ThriftClientInfo(ThriftClient *p, std::mutex *m) : pThriftClient(p), pThriftMutex(m) {}
    };

    struct BusInfo
    {
        BusInfo()
        {
            clientIndex.store(0);
        }

        BusInfo(const BusInfo &info)
        {
            vtHost = info.vtHost;
            vtThriftClientInfo = info.vtThriftClientInfo;
            clientIndex.store(info.clientIndex);
        }

        BusInfo(BusInfo &&info) noexcept
        {
            vtHost = std::move(info.vtHost);
            vtThriftClientInfo = std::move(info.vtThriftClientInfo);
            clientIndex.store(info.clientIndex);
        }

        std::vector<std::string> vtHost;
        std::vector<ThriftClientInfo> vtThriftClientInfo;
        std::atomic<size_t> clientIndex{0};
    };

    struct TaskInfo
    {
        std::string tag;
        std::string topic;
        unsigned long version{0};

        TaskInfo() = default;
        TaskInfo(const std::string &tagName, const std::string &top, unsigned long ver) : tag(tagName), topic(top), version(ver) {}
    };

    using TaskMap = std::unordered_map<std::string, TaskInfo>;
    using TagMap = std::unordered_map<std::string, BusInfo>;
    class BeesContext
    {
    public:
        BeesContext();
        ~BeesContext() = default;

        // 函数原本的设计是只处理 thrift client 连接相关，之所以加了一个 topic 处理是因为可以在 map 查找的时候省掉一次遍历
        bool getNextValidClient(const std::string &taskName, std::string &topic, std::string &tag, ThriftClientInfo &info);

        TaskMap& getWriteMapTask();
        TaskMap& getReadMapTask();
        void swapMap();
        TagMap& getWriteMapTag();
        TagMap& getReadMapTag();

        void swapFlume();
        BusInfo& getWriteFlume();
        BusInfo& getReadFlume();

        bool addTask(const std::string &taskName);
        bool delTask(const std::string &taskName);

        std::string getLocalIp();

        static void releaseClient(std::vector<ThriftClientInfo> &vtThriftClient);

    private:
        size_t getMapWriteIndex();
        size_t getMapReadIndex();

        size_t getFlumeWriteIndex();
        size_t getFlumeReadIndex();

    public:
        Report2BeesInfo m_report2BeesInfo;
        bool m_isTerminate;                                          // 任务线程是否终止
        std::atomic<unsigned int> m_loopInterval;                    // 线程轮询间隔
        std::thread m_beesClusterThread;                             // 定时任务线程，请求 bees 机器列表
        common::HttpClient m_client;                                 // 通用 http 请求
        BusInfo m_vtValidClient[MAX_SIZE];                           // flume 双缓存
        std::atomic<size_t> m_currentFlumeIndex{0};                  // 双缓存当前读索引
        std::atomic<int> m_method{VIVO_BEES_CPP_REPORT_METHOD_BEES}; // 上报方式
        std::vector<std::string> m_vtFlumeIp;                        // flume ip
        int m_flumePort;                                             // flume port
        std::atomic<bool> m_isInitedGlog{false};                     // 是否初始化过 glog
        std::set<std::string> m_setBeesTaskName;                     // 任务列表集合
        std::mutex m_setBeesTaskNameMutex;                           // 任务列表锁
        TaskMap m_mapTask2Tag[MAX_SIZE];                             // 任务到标签的映射
        TagMap m_mapTag2BusInfo[MAX_SIZE];                           // 标签到 bus 的映射
        std::atomic<size_t> m_currentMapIndex{0};                    // 双缓存当前读索引
        std::atomic<unsigned long> m_lastUpdateTime{0};              // 最近更新时间
        std::atomic<size_t> m_failCount{0};                          // 失败次数
    };
}

#endif //BLADE_BASE_COMMON_BEES_CONTEXT_H
