//
// Created by 杨民善 on 2020/6/22.
//

#ifndef BLADE_BASE_COMMON_MONITOR_CONFIG_H
#define BLADE_BASE_COMMON_MONITOR_CONFIG_H

#include "../context/monitor_context.h"

namespace vivo_general_monitor_cpp_report
{
    class MonitorConfig
    {
    public:
        MonitorConfig();
        virtual ~MonitorConfig();

        static void monitorConfigTask(MonitorContext &ctx);

        static int getMonitorConfig(MonitorContext &ctx);

        static int reportSdkVersion(MonitorContext &ctx);
    private:
        static int handleMonitorConfigResponse(MonitorContext &ctx, const std::string &jsonStr);
    };
}

#endif //BLADE_BASE_COMMON_MONITOR_CONFIG_H
