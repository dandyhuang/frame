//
// Created by 杨民善 on 2020/6/22.
//

#ifndef BLADE_BASE_COMMON_MONITOR_CONTEXT_H
#define BLADE_BASE_COMMON_MONITOR_CONTEXT_H

#include "../util/util.h"

#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <map>
#include <atomic>
#include <mutex>
#include <memory>
#include <iomanip>

#include "thirdparty/monitor_sdk_common/net/http/client.h"
#include "thirdparty/monitor_sdk_common/libbeesbus/bees_bus_report.h"
#include "thirdparty/monitor_sdk_common/crypto/hash/murmur/MurmurHash3.h"

#include "thirdparty/boost/lockfree/queue.hpp"
#include <kll_sketch.hpp>

namespace vivo_general_monitor_cpp_report
{
    using ReportMap = std::unordered_map<std::string, std::string>;
    using ReportMapConstIterator = std::unordered_map<std::string, std::string>::const_iterator;

    struct QueueData
    {
        int ruleId;
        long timeStamp;
        ReportMap mapStat;

        QueueData() : ruleId(0), timeStamp(0) {}

        QueueData(const int &id, const long &time, ReportMap &stat) : ruleId(id), timeStamp(time), mapStat(std::move(stat)) {}
    };

    using LockFreeQueue = boost::lockfree::queue<QueueData *, boost::lockfree::fixed_sized<true> > ;
    extern LockFreeQueue *G_P_LOCK_FREE_QUEUE;

    struct MonitorInitInfo
    {
        std::string configUrl;
        std::string token;
        std::vector<int> vtRuleId;
        std::string spaceId;
        bool isAsync;
    };

    struct CommonConfig
    {
        bool update;
        int mapMaxSize;
        int disruptorBufferSize;
        int corePoolSize;
        int beesSendPeriodDefault;
        int selfMonitorId;
        int thriftSendTimeout;
        int thriftReceiveTimeout;

        CommonConfig() : update(false), mapMaxSize(10000), disruptorBufferSize(DEFAULT_QUEUE_SIZE), corePoolSize(1)
        , beesSendPeriodDefault(10), selfMonitorId(101229), thriftSendTimeout(3000), thriftReceiveTimeout(3000) {}
    };

    struct BatchDequeConfig
    {
        size_t batchListSize;           // 单次批量发送大小
        int batchSendPeriod;            // 单次发送最大间隔

        BatchDequeConfig() : batchListSize(500), batchSendPeriod(100) {}
    };

    struct FlumeConfig
    {
        int isFlumeSend;                      // 是否采用flume发送 1-是；0-否
        std::string flumeServerIp;            // flume server ip
        int flumeServerPort;                  // flume server port
        BatchDequeConfig batchDequeConfig;    // 批量发送配置
        FlumeConfig() : isFlumeSend(0), flumeServerIp(""), flumeServerPort(0) {}
    };

    struct BeesConfig
    {
        std::string env;
        std::string managerUrl;
        std::string appName;
        std::string topic;
        std::string beesTaskName;
        unsigned int beesThreadPollingPeriod{0};

        BeesConfig() : env(""), managerUrl(""), appName(""), topic(""), beesTaskName(""), beesThreadPollingPeriod(2 * 60) {}
    };

    struct DimensionInfo
    {
        int id;
        std::string name;

        DimensionInfo() : id(0), name("") {}
    };

    struct MetricInfo
    {
        int id;
        std::string name;
        std::string outputName;
        std::string aggregator;
        int k;
        std::vector<double> vtFractions;
        std::vector<std::string> vtFractionName;

        MetricInfo() : id(0), name(""), outputName(""), aggregator(""), k(0) {}
    };

    struct RuleConfig
    {
        int ruleId;
        int granularity;
        int collectStatus;
        int rollupStatus;
        std::vector<DimensionInfo> vtDimensionInfo;
        std::vector<MetricInfo> vtMetricInfo;
        int earlyMessageRejectionPeriod;
        int lateMessageRejectionPeriod;

        RuleConfig() : ruleId(0), granularity(60), collectStatus(1), rollupStatus(0)
                       , earlyMessageRejectionPeriod(0), lateMessageRejectionPeriod(0) {}
    };

    using RuleMap = std::unordered_map<int, RuleConfig>;

    struct AggregatorKey
    {
        int ruleId;
        long timeStamp;
        std::vector<std::pair<std::string, std::string>> vtDimension;

        AggregatorKey() : ruleId(0), timeStamp(0) {}

        bool operator==(const AggregatorKey &rhs) const
        {
            return ruleId == rhs.ruleId && timeStamp == rhs.timeStamp;
        }

        bool operator<(const AggregatorKey &rhs) const
        {
            return ruleId < rhs.ruleId || timeStamp < rhs.timeStamp;
        }

        friend std::ostream& operator<<(std::ostream &os, const AggregatorKey &value)
        {
            os << std::to_string(value.ruleId) << std::string(", ") << std::to_string(value.timeStamp);
            size_t SIZE(value.vtDimension.size());
            os << std::string("|vtDimension size: ")
               << std::to_string(SIZE)
               << std::string("|");
            for (size_t i = 0; i < SIZE; ++i)
            {
                const std::string &info(value.vtDimension[i].first + ": " + value.vtDimension[i].second + (i < SIZE - 1 ? ", " : ""));
                os << info;
            }
            return os;
        }

        AggregatorKey(const AggregatorKey &rhs)
        {
            ruleId = rhs.ruleId;
            timeStamp = rhs.timeStamp;
            vtDimension = rhs.vtDimension;
        }

        AggregatorKey(AggregatorKey &rhs)
        {
            ruleId = rhs.ruleId;
            timeStamp = rhs.timeStamp;
            vtDimension = std::move(rhs.vtDimension);
        }

        AggregatorKey(AggregatorKey &&rhs) noexcept
        {
            ruleId = rhs.ruleId;
            timeStamp = rhs.timeStamp;
            vtDimension = std::move(rhs.vtDimension);
        }

        AggregatorKey& operator=(AggregatorKey &&rhs)
        {
            ruleId = rhs.ruleId;
            timeStamp = rhs.timeStamp;
            vtDimension = std::move(rhs.vtDimension);

            return *this;
        }
    };

    struct AggregatorKeyHash
    {
        uint64_t operator() (const AggregatorKey &rhs) const
        {
            uint64_t seed = 1;
            uint64_t hashOtpt[2];  // allocate 128 bits
            std::string key(std::to_string(rhs.ruleId) + std::to_string(rhs.timeStamp));
            for (const auto &pair : rhs.vtDimension)
            {
                key += pair.first + pair.second;
            }

            common::MurmurHash3_x64_128(key.c_str(), (uint64_t)key.length(), seed, hashOtpt);

            return hashOtpt[0] + hashOtpt[1];
        }
    };

    using DataSketch = datasketches::kll_sketch<double>;
    struct Metric
    {
        double value;
        std::string name;
        std::string outputName;
        std::string aggregator;
        int k;
        std::vector<double> vtFractions;
        std::vector<std::string> vtFractionName;
        DataSketch sketch;
        int precision;

        Metric() : value(0), name(""), outputName(""), aggregator(""), k(0), precision(0) {}

        bool operator==(const Metric &rhs) const
        {
            return name == name && outputName == outputName;
        }
    };

    struct AggregatorValue
    {
        size_t count;
        std::vector<std::pair<std::string, Metric>> vtMetric;
        AggregatorValue() : count(0) {}
        friend std::ostream& operator<<(std::ostream &os, AggregatorValue &value)
        {
            auto SIZE = value.vtMetric.size();
            os << std::string("|vtMetric size: ") << std::to_string(SIZE) << std::string("|");
            for (size_t i = 0; i < SIZE; ++i)
            {
                const Metric &metric = value.vtMetric[i].second;
                std::ostringstream buf;
                buf.precision(metric.precision);
                buf.setf(std::ios::fixed);
                buf << metric.value;
                os << metric.name << " " << metric.outputName << " " << metric.aggregator
                   << " " << metric.precision << " " << buf.str() << (i < SIZE - 1 ? ", " : "");
            }

            return os;
        }

        AggregatorValue(const AggregatorValue &rhs)
        {
            count = rhs.count;
            vtMetric = rhs.vtMetric;
        }

        AggregatorValue(AggregatorValue &rhs)
        {
            count = rhs.count;
            vtMetric = std::move(rhs.vtMetric);
        }

        AggregatorValue(AggregatorValue &&rhs) noexcept
        {
            count = rhs.count;
            vtMetric = std::move(rhs.vtMetric);
        }

        AggregatorValue& operator=(AggregatorValue &&rhs)
        {
            count = rhs.count;
            vtMetric = std::move(rhs.vtMetric);

            return *this;
        }
    };

    using AggregatorMap = std::unordered_map<AggregatorKey, AggregatorValue, AggregatorKeyHash>;

    struct BufferData
    {
        AggregatorKey key;
        AggregatorValue value;

        BufferData(AggregatorKey &aggregatorKey, AggregatorValue &aggregatorValue)
        {
            key = std::move(aggregatorKey);
            value = std::move(aggregatorValue);
        }
    };

    using LockFreeBufferQueue = boost::lockfree::queue<BufferData *, boost::lockfree::fixed_sized<true> > ;
    extern LockFreeBufferQueue *G_P_LOCK_FREE_BUFFER_QUEUE;

    struct ServerInfo
    {
        std::string ip;
        std::string host;
    };

    struct StatDataInfo
    {
        std::atomic<long> lostByAggregate{0};                  // 维度基数过大超过限制后丢弃的数量
        std::atomic<long> disruptProcessFail{0};               // 数据处理异常的数量
        std::atomic<long> dimensionCount{0};                   // ruleID的维度基数
        std::atomic<long> beesSendFailCount{0};                // bees发送失败的次数/数据条数
        std::atomic<long> flumeSendFailCount{0};               // flume发送失败的次数/数据条数
        std::atomic<long> mapSize{0};                          // SDK中缓存的维度基数大小
        std::atomic<long> addDisruptSuccess{0};                // 放入队列成功的数量
        std::atomic<long> addDisruptFail{0};                   // 放入队列失败的数量
        std::atomic<long> directSendSuccessSize{0};            // 数据直接发送成功的条数
        std::atomic<long> sendMaxTimeByBees{0};                // bees 传输最大耗时
        std::atomic<long> sendMaxTimeByFlume{0};               // flume 传输最大耗时
    };

    class MonitorContext
    {
    public:
        MonitorContext() = default;
        ~MonitorContext() = default;
        int init(const MonitorInitInfo &info);

        size_t getBeesReadIndex()
        {
            return m_beesIndex;
        }

        size_t getBeesWriteIndex()
        {
            return (m_beesIndex + 1) % MAX_SIZE;
        }

        void swapBeesIndex()
        {
            m_beesIndex = (m_beesIndex + 1) % MAX_SIZE;
        }

        size_t getCommonReadIndex()
        {
            return m_commonIndex;
        }

        size_t getCommonWriteIndex()
        {
            return (m_commonIndex + 1) % MAX_SIZE;
        }

        void swapCommonIndex()
        {
            m_commonIndex = (m_commonIndex + 1) % MAX_SIZE;
        }

        size_t getRuleReadIndex()
        {
            return m_ruleIndex;
        }

        size_t getRuleWriteIndex()
        {
            return (m_ruleIndex + 1) % MAX_SIZE;
        }

        void swapRuleIndex()
        {
            m_ruleIndex = (m_ruleIndex + 1) % MAX_SIZE;
        }

        size_t getFlumeReadIndex()
        {
            return m_flumeIndex;
        }

        size_t getFlumeWriteIndex()
        {
            return (m_flumeIndex + 1) % MAX_SIZE;
        }

        void swapFlumeIndex()
        {
            m_flumeIndex = (m_flumeIndex + 1) % MAX_SIZE;
        }

        const std::unordered_map<int, RuleConfig> &getMapRuleId2Config()
        {
            size_t index = getRuleReadIndex();

            return m_mapRuleId2Config[index];
        }

        int isRuleIdValid(const int &ruleId, const long &timeStamp)
        {
            const std::unordered_map<int, RuleConfig> &mapRuleId2Config = getMapRuleId2Config();
            std::unordered_map<int, RuleConfig>::const_iterator iter = mapRuleId2Config.find(ruleId);

            auto isInvalid(mapRuleId2Config.end() != iter ? VERIFY_RULEID_RESULT_SUCCESS : VERIFY_RULEID_RESULT_INVALID);
            if (VERIFY_RULEID_RESULT_SUCCESS == isInvalid && (iter->second.earlyMessageRejectionPeriod > 0 || iter->second.lateMessageRejectionPeriod > 0))
            {
                auto now(NOW_SECONDS.count());
                auto begin(now - iter->second.earlyMessageRejectionPeriod);
                auto end(now + iter->second.lateMessageRejectionPeriod);

                isInvalid = (timeStamp >= begin && timeStamp  < end ? VERIFY_RULEID_RESULT_SUCCESS : VERIFY_RULEID_RESULT_EXPIRED);
                if (VERIFY_RULEID_RESULT_EXPIRED == isInvalid)
                {
                    LOG(ERROR) << "rule id: " << ruleId << " expired! the leagal range is: " << begin
                               << " ~ " << end << " your timeStamp: " << timeStamp << "!" << std::endl;
                }
            }

            return isInvalid;
        }

        bool getRuleConfig(RuleConfig &ruleConfig, const int &ruleId)
        {
            const std::unordered_map<int, RuleConfig> &mapRuleId2Config = getMapRuleId2Config();
            std::unordered_map<int, RuleConfig>::const_iterator iter = mapRuleId2Config.find(ruleId);

            bool isFind(mapRuleId2Config.end() != iter);
            if (isFind)
            {
                ruleConfig = iter->second;
            }

            return isFind;
        }

        bool isRuleIdReport(const int &ruleId, const long &timeStamp)
        {
            const std::unordered_map<int, RuleConfig> &mapRuleId2Config = getMapRuleId2Config();
            auto iter = mapRuleId2Config.find(ruleId);

            auto isReport(true);
            if (mapRuleId2Config.end() != iter && iter->second.earlyMessageRejectionPeriod > 0)
            {
                auto now(NOW_SECONDS.count());
                auto begin(getCollectTime(now - iter->second.earlyMessageRejectionPeriod, iter->second.granularity));

                isReport = (timeStamp < begin);

                if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == m_glogLevel)
                {
                    LOG(INFO) << "begin: " << begin << "|"
                              << "timeStamp: " << timeStamp << "|"
                              << "isReport: " << isReport << "|"
                              << std::endl;
                }
            }

            return isReport;
        }

        void getCommonConfig(CommonConfig &commonConfig)
        {
            size_t index = getCommonReadIndex();

            commonConfig = m_commonConfig[index];
        }

        int getMapMaxSize()
        {
            size_t index = getCommonReadIndex();

            const CommonConfig &commonConfig = m_commonConfig[index];

            return commonConfig.mapMaxSize;
        }

        int getSelfMonitorId()
        {
            size_t index = getCommonReadIndex();

            const CommonConfig &commonConfig = m_commonConfig[index];

            return commonConfig.selfMonitorId;
        }

        void getFlumeConfig(FlumeConfig &flumeConfig)
        {
            size_t index = getFlumeReadIndex();

            flumeConfig = m_FlumeConfig[index];
        }

        int getFlumeSend()
        {
            size_t index = getFlumeReadIndex();

            const FlumeConfig &flumeConfig = m_FlumeConfig[index];

            return flumeConfig.isFlumeSend;
        }

        int getReportInterval()
        {
            size_t index = getCommonReadIndex();
            const CommonConfig &commonConfig(m_commonConfig[index]);

            return commonConfig.beesSendPeriodDefault > 0 ? commonConfig.beesSendPeriodDefault : DEFAULT_REPORT_INTERVAL;
        }

        std::string getBeesTaskName()
        {
            size_t index = getBeesReadIndex();
            const BeesConfig &beesConfig(m_beesConfig[index]);

            return beesConfig.beesTaskName;
        }

        unsigned int getBeesLoopInterval()
        {
            size_t index = getBeesReadIndex();
            const BeesConfig &beesConfig(m_beesConfig[index]);

            return beesConfig.beesThreadPollingPeriod;
        }

        std::string getAppCode()
        {
            size_t index = getBeesReadIndex();
            const BeesConfig &beesConfig(m_beesConfig[index]);

            return beesConfig.appName;
        }

        std::string getEnv()
        {
            size_t index = getBeesReadIndex();
            const BeesConfig &beesConfig(m_beesConfig[index]);

            return beesConfig.env;
        }

        std::string getManagerUrl()
        {
            size_t index = getBeesReadIndex();
            const BeesConfig &beesConfig(m_beesConfig[index]);

            return beesConfig.managerUrl;
        }

        size_t getBatchBufferSize()
        {
            size_t index = getFlumeReadIndex();
            const FlumeConfig &flumeConfig(m_FlumeConfig[index]);

            return flumeConfig.batchDequeConfig.batchListSize;
        }

        long getBatchSendPeriod()
        {
            size_t index = getFlumeReadIndex();
            const FlumeConfig &flumeConfig(m_FlumeConfig[index]);

            return flumeConfig.batchDequeConfig.batchSendPeriod;
        }

        bool insertData2Queue(QueueData &queueData)
        {
            bool insertResult(false);
            if (G_P_LOCK_FREE_QUEUE)
            {
                auto *pData = new QueueData(std::move(queueData));
                if (nullptr != pData)
                {
                    insertResult = G_P_LOCK_FREE_QUEUE->bounded_push(pData);
                    if (!insertResult)
                    {
                        LOG(ERROR) << "delete not insert ruleId: " << pData->ruleId << " " << pData->timeStamp << std::endl;
                        delete pData;
                        pData = nullptr;

                    }
                }
            }
            else
            {
                LOG(ERROR) << "lock free queue is NULL!" << std::endl;
            }

            return insertResult;
        }

        std::mutex* getMutexByThreadId(const std::thread::id &threadId);
        void handleMapStat(const QueueData &queueData, const std::thread::id &threadId);

        void mergeAggregatorValue(AggregatorKey &key, AggregatorValue &value, const std::thread::id &threadId);
        void mergeAggregatorMap(const AggregatorKey &key, AggregatorValue &value, AggregatorMap &aggregatorMap, bool isLimit = false);
        void mergeMetric(std::vector<std::pair<std::string, Metric>> &vtDest, std::vector<std::pair<std::string, Metric>> &vtSource, bool isCommonCalc = true);

        void getHeaders(std::map<std::string, std::string> &headers);

        // 单条数据上报
        int report(AggregatorKey &aggregatorKey, AggregatorValue &aggregatorValue);

        // 多条数据上报
        int report(std::vector<BufferData> &vtBufferData);

        // 写入无需聚合数据到无锁缓存队列
        bool insertBufferData(AggregatorKey &aggregatorKey, AggregatorValue &aggregatorValue);

    public:
        bool m_isTerminate{true};                                    // 读配置任务线程是否终止
        unsigned int m_loopInterval{DEFAULT_LOOP_INTERVAL};          // 线程轮询间隔
        std::thread m_monitorConfigThread;                           // 定时任务线程，请求通用监控配置
        common::HttpClient m_client;                                 // 通用 http 请求
        BeesConfig m_beesConfig[MAX_SIZE];                           // bees 配置
        CommonConfig m_commonConfig[MAX_SIZE];                       // 监控通用配置
        FlumeConfig m_FlumeConfig[MAX_SIZE];                         // flume 相关配置
        RuleMap m_mapRuleId2Config[MAX_SIZE];                        // 从 rule id 到配置的映射
        std::string m_ruleIds;                                       // rule id 集合，以逗号分隔
        std::atomic<size_t> m_beesIndex{0};                          // bees 配置索引
        std::atomic<size_t> m_commonIndex{0};                        // 通用配置索引
        std::atomic<size_t> m_ruleIndex{0};                          // rule 配置索引
        std::atomic<size_t> m_flumeIndex{0};                         // flume 配置索引
        MonitorInitInfo m_monitorInitInfo;                           // 配置信息
        std::vector<std::thread> m_vtCalcThread;                     // 聚合运算线程
        bool m_calcIsTerminate{true};                                // 聚合运算线程是否停止
        std::map<std::thread::id, AggregatorMap> m_mapAggregator;    // 线程 id 到聚合结果的映射
        std::map<std::thread::id, std::mutex*> m_mapMutex;           // 线程 id 到聚合结果锁的映射
        std::atomic<bool> m_reportIsTerminate{true};                 // 上报线程是否停止
        std::thread m_reportThread;                                  // 上报线程
        std::vector<std::thread> m_vtBufferReportThread;             // 不聚合上报线程组
        std::thread m_bufferReportThread;                            // 上报线程
        std::atomic<int> m_glogLevel{vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_ERROR}; // glog 日志级别
        ServerInfo m_serverInfo;                                     // 服务的本地信息
        StatDataInfo m_statInfo;                                     // 自监控相关信息
        std::string m_appCode;                                       // 自监控标识
        AggregatorMap m_bufferAggregatorMap;                         // 上报时不满足时间条件缓存的数据
        AggregatorMap m_destReportAggregatorMap;                     // 用于上报的 map，避免频繁构造
    };
}

#endif //BLADE_BASE_COMMON_MONITOR_CONTEXT_H
