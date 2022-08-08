//
// Created by 杨民善 on 2020/6/20.
//

#include "util/util.h"
#include "monitor_config/monitor_config.h"
#include "general_monitor_report.h"
#include "thread/calc_thread.h"
#include "thread/report_thread.h"

#include <sstream>
#include <unistd.h>

#include "thirdparty/monitor_sdk_common/libbeesbus/bees_bus_report.h"

namespace vivo_general_monitor_cpp_report
{
    MonitorContext GeneralMonitorReport::m_monitorContext;
    std::atomic<bool> GeneralMonitorReport::m_isInit{false};
    int GeneralMonitorReport::init(const std::string &token, const std::vector<int> &vtRuleId, const std::string &spaceId
                                   , const std::string &configUrl, const std::string &appCode, const int &glogLevel
                                   , const std::string &localIp, bool isAsync)
    {
        vivo_bees_bus_cpp_report::BeesBusReport::initGlog(glogLevel);
        m_monitorContext.m_glogLevel = glogLevel;

        if (m_isInit)
        {
            LOG(ERROR) << "monitor has inited! " << std::endl;

            return MONITOR_CPP_REPORT_HAS_INITED;
        }

        if (configUrl.empty() || token.empty() || vtRuleId.empty() || spaceId.empty())
        {
            LOG(ERROR) << "invalid param, configUrl token vtRuleId spaceId maybe empty!" << std::endl;

            return MONITOR_CPP_REPORT_PARAM_INVALID;
        }

        std::ostringstream buf;
        buf << "configUrl: " << configUrl << "|"
            << "token: " << token << "|"
            << "vtRuleId size: " << vtRuleId.size() << "|"
            << "spaceId: " << spaceId << "|"
            << "isAsync: " << isAsync << "|";

        MonitorInitInfo info;
        info.configUrl = configUrl;
        info.token = token;
        info.vtRuleId.assign(vtRuleId.begin(), vtRuleId.end());
        info.spaceId = spaceId;
        info.isAsync = isAsync;
        int ret = m_monitorContext.init(info);
        m_monitorContext.m_appCode = appCode;
        m_monitorContext.m_serverInfo.ip = localIp.empty() ? vivo_bees_bus_cpp_report::getLocalIp() : localIp;
        const size_t HOST_NAME_SIZE{256};
        char hostName[HOST_NAME_SIZE];
        std::string host;
        if (0 == gethostname(hostName, HOST_NAME_SIZE))
        {
            m_monitorContext.m_serverInfo.host = std::string(hostName);
        }

        const size_t &RULE_ID_SIZE(vtRuleId.size());
        for (size_t i = 0; i < RULE_ID_SIZE; ++i)
        {
            m_monitorContext.m_ruleIds += std::to_string(vtRuleId[i]) + (i < RULE_ID_SIZE - 1 ? "," : "");
        }

        buf << "monitorContext init ret: " << ret << "|"
            << "local ip: " << m_monitorContext.m_serverInfo.ip << "|"
            << "local host name: " << m_monitorContext.m_serverInfo.host << "|";
        if (MONITOR_CPP_REPORT_SUCCESS == ret)
        {
            ret = MonitorConfig::getMonitorConfig(m_monitorContext);
            buf << "getMonitorConfig ret: " << ret << "|";
            if (MONITOR_CPP_REPORT_SUCCESS == ret)
            {
                CommonConfig commonConfig;
                m_monitorContext.getCommonConfig(commonConfig);
                const int &thriftSendTimeout(commonConfig.thriftSendTimeout);
                const int &thriftReceiveTimeout(commonConfig.thriftReceiveTimeout);
                // 根据远端配置动态设置上报方式
                FlumeConfig flumeConfig;
                m_monitorContext.getFlumeConfig(flumeConfig);
                std::string taskName = m_monitorContext.getBeesTaskName();
                buf << "isFlumeSend: " << flumeConfig.isFlumeSend << "|"
                    << "flumeServerIp: " << flumeConfig.flumeServerIp << "|"
                    << "flumeServerPort: " << flumeConfig.flumeServerPort << "|"
                    << "taskName: " << taskName << "|";

                if (!taskName.empty())
                {
                    ret = vivo_bees_bus_cpp_report::BeesBusReport::init(m_monitorContext.getManagerUrl(), std::vector<std::string>{taskName}, m_monitorContext.getBeesLoopInterval()
                                                                        , localIp, m_monitorContext.m_glogLevel, vivo_bees_bus_cpp_report::BEES_THRIFT_PORT
                                                                        , thriftSendTimeout, thriftReceiveTimeout, 3, flumeConfig.isFlumeSend, flumeConfig.flumeServerIp
                                                                        , flumeConfig.flumeServerPort);

                    buf << "BeesBusReport::init ret: " << ret << "|";
                }
                else
                {
                    ret = MONITOR_CPP_REPORT_TASK_NAME_EMPTY;
                }


                buf << "thriftSendTimeout: " << thriftSendTimeout << "|"
                    << "thriftReceiveTimeout: " << thriftReceiveTimeout << "|";
                if (ret == vivo_bees_bus_cpp_report::VIVO_BEES_CPP_REPORT_SUCCESS)
                {
                    m_isInit = true;
                    startTask(m_monitorContext);
                }

                auto reportRet = MonitorConfig::reportSdkVersion(m_monitorContext);
                buf << "reportSdkVersion: " << reportRet << "|";
            }
        }

        buf << "final ret: " << ret << "|" << "m_isInit: " << m_isInit << "|"
            << (MONITOR_CPP_REPORT_SUCCESS != ret ? "general monitor init failed!" : "general monitor init success!");
        if (MONITOR_CPP_REPORT_SUCCESS == ret)
        {
            LOG(INFO) << buf.str() << std::endl;
        }
        else
        {
            LOG(ERROR) << buf.str() << std::endl;
        }

        return (ret == vivo_bees_bus_cpp_report::VIVO_BEES_CPP_REPORT_SUCCESS ? MONITOR_CPP_REPORT_SUCCESS : MONITOR_CPP_REPORT_BEES_INIT_FAILED);
    }

    void GeneralMonitorReport::setLogLevel(const int &glogLevel)
    {
        vivo_bees_bus_cpp_report::BeesBusReport::setLogLevel(glogLevel);
        m_monitorContext.m_glogLevel = glogLevel;
    }

    std::string GeneralMonitorReport::getVersion()
    {
        return MONITOR_SDK_VERSION;
    }

    int GeneralMonitorReport::report(const int &ruleId, const long &timeStamp, ReportMap &mapStat)
    {
        if (!m_isInit)
        {
            LOG(ERROR) << "should be init first!" << std::endl;

            return MONITOR_CPP_REPORT_NOT_FINISH_INITED;
        }

        long dataTimestamp = timeStamp;
        if (dataTimestamp <= 0)
        {
            dataTimestamp = NOW_SECONDS.count();
        }

        auto verifyRet(m_monitorContext.isRuleIdValid(ruleId, dataTimestamp));
        if (VERIFY_RULEID_RESULT_SUCCESS != verifyRet)
        {
            LOG(ERROR) << "invalid rule id: " << ruleId << "! verifyRet: " << verifyRet << std::endl;

            return MONITOR_CPP_REPORT_INVALID_RULE_ID;
        }

        const size_t MAP_STAT_SIZE(mapStat.size());
        QueueData queueData(ruleId, dataTimestamp, mapStat);
        bool insertResult = m_monitorContext.insertData2Queue(queueData);
        if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == m_monitorContext.m_glogLevel)
        {
            LOG(INFO) << "ruleId: " << ruleId << "|"
                      << "timeStamp: " << dataTimestamp << "|"
                      << "mapStat size: " << MAP_STAT_SIZE << "|"
                      << "insertResult: " << insertResult << "|"
                      << std::endl;
        }

        // 写入队列成功
        if (insertResult)
        {
            ++m_monitorContext.m_statInfo.addDisruptSuccess;
        }
        else // 写入队列失败
        {
            ++m_monitorContext.m_statInfo.addDisruptFail;
            LOG(ERROR) << "insertResult lock free queue fail!" << std::endl;
        }

        return insertResult ? MONITOR_CPP_REPORT_SUCCESS : MONITOR_CPP_REPORT_FAILED;
    }

    void GeneralMonitorReport::destroy()
    {
        if (m_isInit)
        {
            stopTask();
            vivo_bees_bus_cpp_report::BeesBusReport::destroy();
        }

        vivo_bees_bus_cpp_report::BeesBusReport::glogDestroy();
    }

    int GeneralMonitorReport::startTask(MonitorContext &ctx)
    {
        m_monitorContext.m_isTerminate = false;
        m_monitorContext.m_monitorConfigThread = std::thread(MonitorConfig::monitorConfigTask, std::ref(m_monitorContext));

        std::ostringstream buf;
        CommonConfig commonConfig;
        ctx.getCommonConfig(commonConfig);
        // 根据通用配置初始化无锁队列和启动聚合运算线程
        const int THREAD_SIZE(commonConfig.corePoolSize > MAX_CALC_THREAD_SIZE ? MAX_CALC_THREAD_SIZE
                              : (commonConfig.corePoolSize < MIN_CALC_THREAD_SIZE ? MIN_CALC_THREAD_SIZE : commonConfig.corePoolSize));
        const int LOCK_FREE_SIZE(commonConfig.disruptorBufferSize > 0 ? (commonConfig.disruptorBufferSize > MAX_QUEUE_SIZE ? MAX_QUEUE_SIZE : commonConfig.disruptorBufferSize) : DEFAULT_QUEUE_SIZE);
        ctx.m_calcIsTerminate = false;
        buf << "corePoolSize: " << commonConfig.corePoolSize << "|"
            << "THREAD_SIZE: " << THREAD_SIZE << "|"
            << "disruptorBufferSize: " << commonConfig.disruptorBufferSize << "|"
            << "LOCK_FREE_SIZE: " << LOCK_FREE_SIZE << "|";
        // 分配无锁队列
        if (!G_P_LOCK_FREE_QUEUE)
        {
            buf << "new G_P_LOCK_FREE_QUEUE|";
            G_P_LOCK_FREE_QUEUE = new LockFreeQueue(LOCK_FREE_SIZE);
        }
        else
        {
            LOG(ERROR) << "free lock queue G_P_LOCK_FREE_QUEUE is not null!" << std::endl;
        }

        // 分配直接上报缓存无锁队列
        if (!G_P_LOCK_FREE_BUFFER_QUEUE)
        {
            buf << "new G_P_LOCK_FREE_BUFFER_QUEUE|";
            G_P_LOCK_FREE_BUFFER_QUEUE = new LockFreeBufferQueue(LOCK_FREE_SIZE);
        }
        else
        {
            LOG(ERROR) << "free lock queue G_P_LOCK_FREE_BUFFER_QUEUE is not null!" << std::endl;
        }

        ctx.m_reportIsTerminate = false;
        for (int i = 0; i < THREAD_SIZE; ++i)
        {
            // 启动计算线程并初始化计算map
            std::thread calcThread = std::thread(CalcThread::calcTask, std::ref(m_monitorContext));
            ctx.m_mapMutex[calcThread.get_id()] = new std::mutex();
            ctx.m_mapAggregator[calcThread.get_id()] = std::move(AggregatorMap());
            ctx.m_vtCalcThread.push_back(std::move(calcThread));

            // 启动非聚合上报线程组
            //std::thread bufferReportThread = std::thread(ReportThread::bufferReportTask, std::ref(m_monitorContext));
            //ctx.m_vtBufferReportThread.push_back(std::move(bufferReportThread));
        }

        // 启动上报线程
        ctx.m_reportThread = std::thread(ReportThread::reportTask, std::ref(m_monitorContext));
        ctx.m_bufferReportThread = std::thread(ReportThread::bufferReportTask, std::ref(m_monitorContext));


        buf << "m_mapMutex size: " << ctx.m_mapMutex.size() << "|"
            << "m_vtCalcThread size: " << ctx.m_vtCalcThread.size() << "|"
            << "m_calcIsTerminate: " << ctx.m_calcIsTerminate << "|"
            << "m_reportIsTerminate: " << ctx.m_reportIsTerminate << "|";

        LOG(INFO) << buf.str() << std::endl;

        return (!ctx.m_mapMutex.empty() && !ctx.m_vtCalcThread.empty() ? MONITOR_CPP_REPORT_SUCCESS : MONITOR_CPP_REPORT_FAILED);
    }

    void GeneralMonitorReport::stopTask()
    {
        std::ostringstream buf;
        buf << "m_calcIsTerminate: " << m_monitorContext.m_calcIsTerminate << "|";
        // 停止计算线程
        if (!m_monitorContext.m_calcIsTerminate)
        {
            size_t threadCount(0);
            m_monitorContext.m_calcIsTerminate = true;
            const size_t THREAD_SIZE(m_monitorContext.m_vtCalcThread.size());
            for (size_t i = 0; i < THREAD_SIZE; ++i)
            {
                std::thread &calcThread = m_monitorContext.m_vtCalcThread[i];
                calcThread.join();
                ++threadCount;
            }

            m_monitorContext.m_vtCalcThread.clear();
            buf << "join calc thread count: " << threadCount << "|";
        }

        buf << "m_reportIsTerminate: " << m_monitorContext.m_reportIsTerminate << "|";
        // 停止上报线程
        if (!m_monitorContext.m_reportIsTerminate)
        {
            m_monitorContext.m_reportIsTerminate = true;
            m_monitorContext.m_reportThread.join();
            m_monitorContext.m_bufferReportThread.join();

            // 停止非聚合上报线程组
            size_t threadCount(0);
            const size_t THREAD_SIZE(m_monitorContext.m_vtBufferReportThread.size());
            for (size_t i = 0; i < THREAD_SIZE; ++i)
            {
                std::thread &bufferThread = m_monitorContext.m_vtBufferReportThread[i];
                bufferThread.join();
                ++threadCount;
            }

            m_monitorContext.m_vtBufferReportThread.clear();
            buf << "join buffer report thread count: " << threadCount << "|";
        }

        buf << "m_isTerminate: " << m_monitorContext.m_isTerminate << "|";
        // 停止配置线程
        if (!m_monitorContext.m_isTerminate)
        {
            m_monitorContext.m_isTerminate = true;
            m_monitorContext.m_monitorConfigThread.join();
        }

        // 释放互斥变量
        size_t releaseMutexCount(0);
        std::map<std::thread::id, std::mutex*>::iterator iter = m_monitorContext.m_mapMutex.begin();
        while (iter != m_monitorContext.m_mapMutex.end())
        {
            if (iter->second)
            {
                delete iter->second;
                iter->second = NULL;
                ++releaseMutexCount;
            }

            ++iter;
        }

        m_monitorContext.m_mapMutex.clear();
        buf << "releaseMutexCount: " << releaseMutexCount << "|";

        m_monitorContext.m_mapAggregator.clear();
        m_monitorContext.m_bufferAggregatorMap.clear();
        m_monitorContext.m_destReportAggregatorMap.clear();

        // 释放未处理的原始缓存数据
        if (G_P_LOCK_FREE_QUEUE)
        {
            buf << "release G_P_LOCK_FREE_QUEUE|";
            releaseQueueData();
            delete G_P_LOCK_FREE_QUEUE;
            G_P_LOCK_FREE_QUEUE = NULL;
        }

        // 释放未处理的非聚合缓存数据
        if (G_P_LOCK_FREE_BUFFER_QUEUE)
        {
            buf << "release G_P_LOCK_FREE_BUFFER_QUEUE|";
            releaseBufferData();
            delete G_P_LOCK_FREE_BUFFER_QUEUE;
            G_P_LOCK_FREE_BUFFER_QUEUE = NULL;
        }

        LOG(INFO) << buf.str() << std::endl;
    }

    void GeneralMonitorReport::releaseQueueData()
    {
        size_t releaseQueueCount(0);
        QueueData *pQueueData = NULL;
        while (G_P_LOCK_FREE_QUEUE && G_P_LOCK_FREE_QUEUE->pop(pQueueData))
        {
            if (pQueueData)
            {
                delete pQueueData;
                pQueueData = NULL;

                ++releaseQueueCount;
            }
        }

        LOG(INFO) << "releaseQueueCount: " << releaseQueueCount << std::endl;
    }

    void GeneralMonitorReport::releaseBufferData()
    {
        size_t releaseBufferCount(0);
        QueueData *pBufferData = NULL;
        while (G_P_LOCK_FREE_BUFFER_QUEUE && G_P_LOCK_FREE_BUFFER_QUEUE->pop(pBufferData))
        {
            if (pBufferData)
            {
                delete pBufferData;
                pBufferData = NULL;

                ++releaseBufferCount;
            }
        }

        LOG(INFO) << "releaseBufferCount: " << releaseBufferCount << std::endl;
    }
}