//
// Created by 杨民善 on 2020/7/6.
//

#ifndef BLADE_BASE_COMMON_REPORT_THREAD_H
#define BLADE_BASE_COMMON_REPORT_THREAD_H

#include "../context/monitor_context.h"

namespace vivo_general_monitor_cpp_report
{
    class ReportThread
    {
    public:
        static void reportTask(MonitorContext &ctx);
        static void bufferReportTask(MonitorContext &ctx);

    private:
        static void mergeMapAndReport(MonitorContext &ctx);
        static int batchReport(MonitorContext &ctx, AggregatorMap &aggregatorMap);
        static int report(MonitorContext &ctx, AggregatorMap &aggregatorMap);
        static void reportStat(MonitorContext &ctx);
        static void getReportAggregatorMap(MonitorContext &ctx, AggregatorMap &destAggregatorMap);
    };
}

#endif //BLADE_BASE_COMMON_REPORT_THREAD_H
