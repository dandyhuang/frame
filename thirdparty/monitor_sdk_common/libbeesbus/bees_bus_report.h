//
// Created by 杨民善 on 2020/6/15.
//

#ifndef BLADE_BASE_COMMON_BEES_BUS_REPORT_H
#define BLADE_BASE_COMMON_BEES_BUS_REPORT_H

#include <iostream>
#include <string>
#include <vector>

#include "report/report_to_bees.h"
#include "log/glog_helper.h"

namespace vivo_bees_bus_cpp_report
{
    class BeesBusReport
    {
    public:
        BeesBusReport(){}
        virtual ~BeesBusReport() {}

        static int init(const std::string &beesManagerUrl, const std::vector<std::string> &vtBeesTaskName, const unsigned int &beesThreadPollingPeriod = DEFAULT_INTERVAL
                        , const std::string &localIp = "", const int &glogLevel = VIVO_BEES_GLOG_LEVEL_ERROR, const int &beesBusPort = BEES_THRIFT_PORT
                        , const int &sendTimeout = DEFAULT_THRIFT_TIMEOUT, const int &recvTimeout = DEFAULT_THRIFT_TIMEOUT
                        , const size_t &retryMax = DEFAULT_THRIFT_RETRY_MAX, const int &method = 0, const std::string &flumeIp = ""
                        , const int &flumePort = 0);
        static void initGlog(const int &glogLevel = VIVO_BEES_GLOG_LEVEL_ERROR);
        static void glogDestroy();
        static void setLogLevel(const int &glogLevel = VIVO_BEES_GLOG_LEVEL_ERROR);
        static void setLoopInterval(const unsigned int &beesThreadPollingPeriod);
        static void setFlumeConfig(const std::string &flumeIp, const int &flumePort);
        static std::string getSdkVersion();
        static void setReportMethod(const int &method = VIVO_BEES_CPP_REPORT_METHOD_BEES);
        static bool addTask(const std::string &taskName);
        static bool delTask(const std::string &taskName);
        static BeesBusReportStatus append(const AppendContext &appendContext, BeesBusThriftFlumeEvent &event);
        static BeesBusReportStatus appendBatch(const AppendContext &appendContext, std::vector<BeesBusThriftFlumeEvent> &events);
        static void destroy();

    private:
        static BeesContext m_beesContext;
        static std::atomic<bool> m_isInit;
    };
}

#endif //BLADE_BASE_COMMON_BEES_BUS_REPORT_H
