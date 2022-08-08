//
// Created by 杨民善 on 2020/6/15.
//

#include "bees_bus_report.h"
#include "report/report_to_bees.h"

namespace vivo_bees_bus_cpp_report
{
    BeesContext BeesBusReport::m_beesContext;
    std::atomic<bool> BeesBusReport::m_isInit{false};

    int BeesBusReport::init(const std::string &beesManagerUrl, const std::vector<std::string> &vtBeesTaskName, const unsigned int &beesThreadPollingPeriod
                            , const std::string &localIp, const int &glogLevel, const int &beesBusPort, const int &sendTimeout, const int &recvTimeout
                            , const size_t &retryMax, const int &method, const std::string &flumeIp, const int &flumePort)
    {
        initGlog(glogLevel);

        if (m_isInit)
        {
            LOG(ERROR) << "bees bus sdk have inited already!" << std::endl;
            return VIVO_BEES_CPP_REPORT_HAS_INITED;
        }

        if (beesManagerUrl.empty() || vtBeesTaskName.empty())
        {
            LOG(ERROR) << "beesManagerUrl: " << beesManagerUrl << "|"
                       << "vtBeesTaskName size: " << vtBeesTaskName.size() <<std::endl;

            return VIVO_BEES_CPP_REPORT_PARAM_EMPTY;
        }

        m_beesContext.m_setBeesTaskName.insert(vtBeesTaskName.begin(), vtBeesTaskName.end());

        m_beesContext.m_report2BeesInfo.localIp = localIp;
        if (m_beesContext.m_report2BeesInfo.localIp.empty())
        {
            m_beesContext.getLocalIp();
        }

        m_beesContext.m_report2BeesInfo.beesManagerUrl = beesManagerUrl;
        m_beesContext.m_report2BeesInfo.sendTimeout = sendTimeout;
        m_beesContext.m_report2BeesInfo.recvTimeout = recvTimeout;
        m_beesContext.m_report2BeesInfo.retryMax = retryMax;
        m_beesContext.m_report2BeesInfo.beesBusPort = beesBusPort;
        m_beesContext.m_loopInterval = beesThreadPollingPeriod > MIN_DEFAULT_INTERVAL ? beesThreadPollingPeriod : MIN_DEFAULT_INTERVAL;

        setFlumeConfig(flumeIp, flumePort);
        setReportMethod(method);
        int ret = Report2Bees::refreshClusterList(m_beesContext);

        if (VIVO_BEES_CPP_REPORT_SUCCESS == ret)
        {
            m_beesContext.m_isTerminate = false;
            m_beesContext.m_beesClusterThread = std::thread(Report2Bees::beesBusTask, std::ref(m_beesContext));

            m_isInit = true;

            LOG(INFO) << "bees bus sdk init success!" << std::endl
                      << "local ip: " << m_beesContext.m_report2BeesInfo.localIp << std::endl
                      << "bees manager url: " << m_beesContext.m_report2BeesInfo.beesManagerUrl << std::endl
                      << "bees bus port: " << m_beesContext.m_report2BeesInfo.beesBusPort << std::endl
                      << "terminate: " << m_beesContext.m_isTerminate << std::endl
                      << "thrift send timeout: " << m_beesContext.m_report2BeesInfo.sendTimeout << std::endl
                      << "thrift recv timeout: " << m_beesContext.m_report2BeesInfo.recvTimeout << std::endl
                      << "thrift retry max: " << m_beesContext.m_report2BeesInfo.retryMax << std::endl
                      << "method: " << method << std::endl
                      << "flumeIp: " << flumeIp << std::endl
                      << "flumePort: " << flumePort << std::endl;
        }

        return ret;
    }

    void BeesBusReport::initGlog(const int &glogLevel)
    {
        if (!m_beesContext.m_isInitedGlog && glogLevel >= 0)
        {
            m_beesContext.m_isInitedGlog = true;
            GlogHelper::init(const_cast<char *>(BEES_BUS_CPP_REPORT_APP_NAME.c_str()), glogLevel);
        }
    }

    void BeesBusReport::glogDestroy()
    {
        if (m_beesContext.m_isInitedGlog)
        {
            m_beesContext.m_isInitedGlog = false;
            GlogHelper::destroy();
        }
    }

    void BeesBusReport::setLogLevel(const int &glogLevel)
    {
        GlogHelper::setGlogLevel(glogLevel);
    }

    void BeesBusReport::setLoopInterval(const unsigned int &beesThreadPollingPeriod)
    {
        if (beesThreadPollingPeriod == m_beesContext.m_loopInterval)
        {
            return;
        }

        m_beesContext.m_loopInterval = beesThreadPollingPeriod > MIN_DEFAULT_INTERVAL ? beesThreadPollingPeriod : MIN_DEFAULT_INTERVAL;
    }

    void BeesBusReport::setFlumeConfig(const std::string &flumeIp, const int &flumePort)
    {
        m_beesContext.m_vtFlumeIp.emplace_back(flumeIp);
        m_beesContext.m_flumePort = flumePort;

        LOG(INFO) << "flumeIp: " << flumeIp << "|"
                  << "flumePort: " << flumePort << "|"
                  << std::endl;
    }

    inline std::string BeesBusReport::getSdkVersion()
    {
        return BEES_SDK_VERSION;
    }

    void BeesBusReport::setReportMethod(const int &method)
    {
        LOG(INFO) << "m_method: " << m_beesContext.m_method << "|"
                  << "method: " << method << "|"
                  << std::endl;
        if (m_beesContext.m_method != method)
        {
            m_beesContext.m_method = method;
        }
    }

    bool BeesBusReport::addTask(const std::string &taskName)
    {
        if (!m_isInit)
        {
            return false;
        }

        return m_beesContext.addTask(taskName);
    }

    bool BeesBusReport::delTask(const std::string &taskName)
    {
        if (!m_isInit)
        {
            return false;
        }

        return m_beesContext.delTask(taskName);
    }

    BeesBusReportStatus BeesBusReport::append(const AppendContext &appendContext, BeesBusThriftFlumeEvent &event)
    {
        int ret{VIVO_BEES_CPP_REPORT_NOT_FINISH_INIT};
        if (m_isInit)
        {
            int count{0};
            while (com::vivo::bees::bus::thrift::Status::OK != (ret = Report2Bees::append(m_beesContext, appendContext, event))
                   && count < m_beesContext.m_report2BeesInfo.retryMax)
            {
                ++count;
            }

            LOG(INFO) << "append invoke count: " << count << "|"
                      << "status: " << ret << "|"
                      << std::endl;
        }
        else
        {
            LOG(ERROR) << "must be finish init first! isInit: " << m_isInit << "|"
                       << std::endl;
        }

        return static_cast<BeesBusReportStatus>(ret);
    }

    BeesBusReportStatus BeesBusReport::appendBatch(const AppendContext &appendContext, std::vector<BeesBusThriftFlumeEvent> &events)
    {
        int ret{VIVO_BEES_CPP_REPORT_NOT_FINISH_INIT};
        if (m_isInit)
        {
            int count{0};
            while (com::vivo::bees::bus::thrift::Status::OK != (ret = Report2Bees::appendBatch(m_beesContext, appendContext, events))
                   && count < m_beesContext.m_report2BeesInfo.retryMax)
            {
                ++count;
            }

            LOG(INFO) << "appendBatch invoke count: " << count << "|"
                      << "status: " << ret << "|"
                      << std::endl;
        }
        else
        {
            LOG(ERROR) << "must be finish init first! isInit: " << m_isInit << "|"
                       << std::endl;
        }

        return static_cast<BeesBusReportStatus>(ret);
    }

    void BeesBusReport::destroy()
    {
        if (m_isInit)
        {
            m_isInit = false;

            m_beesContext.m_isTerminate = true;

            m_beesContext.m_beesClusterThread.join();

            BeesContext::releaseClient(m_beesContext.getWriteFlume().vtThriftClientInfo);
            BeesContext::releaseClient(m_beesContext.getReadFlume().vtThriftClientInfo);

            auto releaseMapTag = [](TagMap &mapTag) {
                auto tagIter = mapTag.begin();
                while (mapTag.end() != tagIter)
                {
                    BeesContext::releaseClient(tagIter->second.vtThriftClientInfo);
                    ++tagIter;
                }

                mapTag.clear();
            };

            releaseMapTag(m_beesContext.getWriteMapTag());
            releaseMapTag(m_beesContext.getReadMapTag());

            m_beesContext.m_vtFlumeIp.clear();
            std::lock_guard<std::mutex> lockGuard(m_beesContext.m_setBeesTaskNameMutex);
            m_beesContext.m_setBeesTaskName.clear();

            LOG(INFO) << "bees bus sdk destroy success!" << std::endl;
        }

        glogDestroy();
    }
}