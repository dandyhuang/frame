//
// Created by 杨民善 on 2020/6/15.
//

#ifndef BLADE_BASE_COMMON_REPORT_TO_BEES_H
#define BLADE_BASE_COMMON_REPORT_TO_BEES_H

#include "../context/bees_context.h"
#include "../thrift/ThriftSourceProtocol.h"

namespace vivo_bees_bus_cpp_report
{
    using BeesBusReportStatus = com::vivo::bees::bus::thrift::Status::type;
    using BeesBusThriftFlumeEvent = com::vivo::bees::bus::thrift::ThriftFlumeEvent;
    class Report2Bees
    {
    public:
        Report2Bees() = default;
        virtual ~Report2Bees() = default;
        static void beesBusTask(BeesContext &ctx);
        static int refreshClusterList(BeesContext &ctx);
        static BeesBusReportStatus append(BeesContext &ctx, const AppendContext &appendContext, BeesBusThriftFlumeEvent &event);
        static BeesBusReportStatus appendBatch(BeesContext &ctx, const AppendContext &appendContext, std::vector<BeesBusThriftFlumeEvent> &events);

    private:
        static bool isNeedUpdate(BeesContext &ctx);
        static bool reviseTask(BeesContext &ctx);
        static bool reviseTag(BeesContext &ctx, const std::set<std::string> &setTag);
        static void checkAndRebuildConnection(BeesContext &ctx);
        static void rebuildConnection(BeesContext &ctx);
        static int getBusListInfo(BeesContext &ctx);
        static int getFlumeListInfo(BeesContext &ctx);
        static bool handleBusInfoResponse(BeesContext &ctx, const std::string &jsonStr);
        static int buildConnection(BeesContext &ctx, std::vector<ThriftClientInfo> &vtThriftClient, std::vector<std::string> &vtHost, int port);
        static void fillHeader(BeesContext &ctx, const std::string &task, const std::string &topic, const std::string &tag, BeesBusThriftFlumeEvent &event);
    };
}

#endif //BLADE_BASE_COMMON_REPORT_TO_BEES_H
