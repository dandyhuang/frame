//
// Created by 杨民善 on 2020/7/6.
//

#include <iomanip>

#include "../util/tc_base64.h"
#include "report_thread.h"

#include "thirdparty/rapidjson/document.h"
#include "thirdparty/rapidjson/stringbuffer.h"
#include "thirdparty/rapidjson/writer.h"
#include "../../libbeesbus/thrift/bus_types.h"

namespace vivo_general_monitor_cpp_report
{
    void ReportThread::reportTask(MonitorContext &ctx)
    {
        std::chrono::seconds now = NOW_SECONDS;
        long lastReportTime(now.count());
        const int statReportInterval(60);
        long lastStatReportTime(now.count());
        while (!ctx.m_reportIsTerminate)
        {
            // 业务上报
            int loopInterval(ctx.getReportInterval());
            now = NOW_SECONDS;
            if (now.count() - lastReportTime > loopInterval)
            {
                lastReportTime = now.count();
                mergeMapAndReport(ctx);
            }

            // 自监控上报
            if (now.count() - lastStatReportTime > statReportInterval)
            {
                lastStatReportTime = now.count();

                reportStat(ctx);
            }

            std::this_thread::sleep_for(std::chrono::seconds(DEFAULT_SECOND));
        }
    }

    void ReportThread::bufferReportTask(MonitorContext &ctx)
    {
        std::vector<BufferData> vtBuffer;
        std::chrono::milliseconds now = NOW_MILLI_SECONDS;
        long lastReportTime(now.count());
        while (!ctx.m_reportIsTerminate)
        {
            // 缓存汇总数据上报
            BufferData *pBufferData = NULL;
            while (G_P_LOCK_FREE_BUFFER_QUEUE && G_P_LOCK_FREE_BUFFER_QUEUE->pop(pBufferData))
            {
                if (pBufferData)
                {
                    vtBuffer.emplace_back(*pBufferData);
                    delete pBufferData;
                    pBufferData = NULL;
                }
                else
                {
                    ++ctx.m_statInfo.disruptProcessFail;

                    LOG(ERROR) << "pBufferData is null may be alloc memory failed!" << std::endl;
                }

                // 超过批量发送大小则发送
                if (vtBuffer.size() >= ctx.getBatchBufferSize())
                {
                    ctx.report(vtBuffer);
                    now = NOW_MILLI_SECONDS;
                    lastReportTime = now.count();
                    vtBuffer.clear();
                }
            }

            // 若发送不足设置的批量数则超过最大发送间隔也发送
            if (!vtBuffer.empty())
            {
                now = NOW_MILLI_SECONDS;
                if (now.count() - lastReportTime >= ctx.getBatchSendPeriod())
                {
                    ctx.report(vtBuffer);
                    lastReportTime = now.count();
                    vtBuffer.clear();
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_MILLI_SECONDS));
        }
    }

    void ReportThread::mergeMapAndReport(MonitorContext &ctx)
    {
        // 将所有单个线程计算的结果汇总上报
        std::map<std::thread::id, AggregatorMap> &mapAggregator(ctx.m_mapAggregator);
        std::map<std::thread::id, AggregatorMap>::iterator aggregatorIter = ctx.m_mapAggregator.begin();
        AggregatorMap sourceAggregatorMap;
        AggregatorMap &destReportAggregatorMap = ctx.m_destReportAggregatorMap;
        getReportAggregatorMap(ctx, destReportAggregatorMap);
        while (mapAggregator.end() != aggregatorIter)
        {
            std::map<std::thread::id, std::mutex*> &mapMutex(ctx.m_mapMutex);
            std::map<std::thread::id, std::mutex*>::iterator mutexIter = mapMutex.find(aggregatorIter->first);
            if (mapMutex.end() != mutexIter)
            {
                sourceAggregatorMap.clear();
                if (mutexIter->second)
                {
                    // 与计算线程互斥
                    std::lock_guard<std::mutex> guard(*mutexIter->second);
                    sourceAggregatorMap = std::move(aggregatorIter->second);
                }
                else
                {
                    LOG(ERROR) << "threadId: " << aggregatorIter->first << "|"
                               << "get mutex null!" << "|"
                               << std::endl;
                }

                AggregatorMap::iterator sourceIter = sourceAggregatorMap.begin();
                while (sourceAggregatorMap.end() != sourceIter)
                {
                    const AggregatorKey &key = sourceIter->first;
                    AggregatorValue &value = sourceIter->second;
                    if (ctx.isRuleIdReport(key.ruleId, key.timeStamp))
                    {
                        ctx.mergeAggregatorMap(key, value, destReportAggregatorMap);
                    }
                    else
                    {
                        ctx.mergeAggregatorMap(key, value, ctx.m_bufferAggregatorMap);
                    }

                    ++sourceIter;
                }
            }
            else
            {
                LOG(ERROR) << "threadId: " << aggregatorIter->first << "|"
                           << "get mutex error!" << "|"
                           << std::endl;
            }

            ++aggregatorIter;
        }

        if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == ctx.m_glogLevel)
        {
            LOG(INFO) << "ctx.m_bufferAggregatorMap size: " << ctx.m_bufferAggregatorMap.size() << "|"
                      << "destAggregatorMap size: " << destReportAggregatorMap.size() << "|"
                      << std::endl;
        }

        if (!destReportAggregatorMap.empty())
        {
            ctx.m_statInfo.mapSize += destReportAggregatorMap.size();
            batchReport(ctx, destReportAggregatorMap);
            destReportAggregatorMap.clear();
        }
    }

    int ReportThread::batchReport(MonitorContext &ctx, AggregatorMap &aggregatorMap)
    {
        int ret(com::vivo::bees::bus::thrift::Status::UNKNOWN);
        AggregatorMap batchMap;
        auto iter = aggregatorMap.begin();
        while (aggregatorMap.end() != iter)
        {
            if (batchMap.size() >= ctx.getBatchBufferSize())
            {
                ret = report(ctx, batchMap);
                batchMap.clear();
            }

            batchMap[iter->first] = std::move(iter->second);
            ++iter;
        }

        if (!batchMap.empty())
        {
            ret = report(ctx, batchMap);
            batchMap.clear();
        }

        return ret;
    }

    int ReportThread::report(MonitorContext &ctx, AggregatorMap &aggregatorMap)
    {
        int ret(com::vivo::bees::bus::thrift::Status::UNKNOWN);
        rapidjson::Document doc;
        doc.SetArray();
        rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();
        AggregatorMap::iterator iter = aggregatorMap.begin();

        auto reportToServer = [&](rapidjson::Document &document) {
            vivo_bees_bus_cpp_report::BeesBusThriftFlumeEvent event;

            rapidjson::StringBuffer strBuf;
            rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
            document.Accept(writer);

            ctx.getHeaders(event.headers);
            const std::string &body(strBuf.GetString());
            event.__set_body(body);

            if (!event.body.empty())
            {
                std::chrono::milliseconds now = NOW_MILLI_SECONDS;
                long begin(now.count());
                ret = vivo_bees_bus_cpp_report::BeesBusReport::append(vivo_bees_bus_cpp_report::AppendContext{ctx.getBeesTaskName()}, event);
                now = NOW_MILLI_SECONDS;
                long diff(now.count() - begin);
                switch (ctx.getFlumeSend())
                {
                    case 0:
                        if (0 != ret)
                        {
                            ++ctx.m_statInfo.beesSendFailCount;
                        }
                        else
                        {
                            if (diff > ctx.m_statInfo.sendMaxTimeByBees)
                            {
                                ctx.m_statInfo.sendMaxTimeByBees = diff;
                            }
                        }
                        break;
                    case 1:
                        if (0 != ret)
                        {
                            ++ctx.m_statInfo.flumeSendFailCount;
                        }
                        else
                        {
                            if (diff > ctx.m_statInfo.sendMaxTimeByFlume)
                            {
                                ctx.m_statInfo.sendMaxTimeByFlume = diff;
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
            else
            {
                ++ctx.m_statInfo.disruptProcessFail;

                LOG(ERROR) << "report failed! event body: " << body << "|"
                           << "append ret: " << ret << "|"
                           << std::endl;
            }

            if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == ctx.m_glogLevel)
            {
                LOG(INFO) << "event body: " << event.body << "|"
                          << "append ret: " << ret << "|"
                          << std::endl;
            }
        };

        while (aggregatorMap.end() != iter)
        {
            const AggregatorKey &key = iter->first;
            rapidjson::Value obj(rapidjson::kObjectType);
            obj.AddMember("rule_id", key.ruleId, allocator);
            obj.AddMember("timestamp", key.timeStamp * PER_SECOND_MILLI, allocator);

            AggregatorValue &value = iter->second;
            size_t eventCount(value.count > 0 ? value.count - 1 : 0);
            obj.AddMember("eventCount", eventCount, allocator);
            // 维度
            const std::vector<std::pair<std::string, std::string>> &vtDimension = key.vtDimension;
            const size_t &DIMEN_SIZE(vtDimension.size());
            for (size_t j = 0; j < DIMEN_SIZE; ++j)
            {
                obj.AddMember(rapidjson::Value().SetString(vtDimension[j].first.c_str(), vtDimension[j].first.length(), allocator)
                        , rapidjson::Value().SetString(vtDimension[j].second.c_str(), vtDimension[j].second.length(), allocator)
                        , allocator);
            }

            ctx.m_statInfo.dimensionCount += DIMEN_SIZE;

            // 指标
            const std::vector<std::pair<std::string, Metric>> &vtMetric = value.vtMetric;
            const size_t &METRIC_SIZE(vtMetric.size());
            for (size_t k = 0; k < METRIC_SIZE; ++k)
            {
                const auto &metric = vtMetric[k].second;
                if (AGGREGATOR_QUANTILE == metric.aggregator)
                {
                    if (!metric.vtFractions.empty()) // 上报分位值
                    {
                        const size_t FRACTIONS_SIZE(metric.vtFractions.size());
                        const double *fractions = metric.vtFractions.data();
                        auto vtQuantiles = metric.sketch.get_quantiles(fractions, FRACTIONS_SIZE);
                        std::ostringstream buf;
                        for (size_t j = 0; j < FRACTIONS_SIZE; ++j)
                        {
                            buf.str("");
                            buf.precision(metric.precision);
                            buf.setf(std::ios::fixed);
                            std::string outputName(metric.outputName + metric.vtFractionName[j]);
                            buf << vtQuantiles[j];
                            std::string quantile(buf.str());
                            obj.AddMember(rapidjson::Value().SetString(outputName.c_str(), outputName.length(), allocator)
                                          , rapidjson::Value().SetString(quantile.c_str(), quantile.length(), allocator), allocator);

                            if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == ctx.m_glogLevel)
                            {
                                LOG(INFO) << "rule_id: " << key.ruleId << "|"
                                          << "timestamp: " << key.timeStamp * PER_SECOND_MILLI << "|"
                                          << "outputName: " << outputName << "|"
                                          << "quantile: " << quantile << "|"
                                          << std::endl;
                            }
                        }
                    }
                    else // 上报摘要数据，与 java 有兼容问题，暂不支持
                    {
                        std::ostringstream os;
                        metric.sketch.serialize(os);
                        const std::string &dataSketch(TC_Base64::encode(os.str()));
                        obj.AddMember(rapidjson::Value().SetString(metric.outputName.c_str(), metric.outputName.length(), allocator)
                                , rapidjson::Value().SetString(dataSketch.c_str(), dataSketch.length(), allocator), allocator);

                        if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == ctx.m_glogLevel)
                        {
                            const size_t MAX_FRACTIONS_SIZE{3};
                            const int INVALID{-1};
                            const double fractions[MAX_FRACTIONS_SIZE] {0, 0.5, 1};
                            auto quantiles = metric.sketch.get_quantiles(fractions, MAX_FRACTIONS_SIZE);
                            LOG(INFO) << "rule_id: " << key.ruleId << "|"
                                      << "timestamp: " << key.timeStamp * PER_SECOND_MILLI << "|"
                                      //<< "dataSketch: " << dataSketch << "|"
                                      << "min: " << (!quantiles.empty() ? quantiles[0] : INVALID) << "|"
                                      << "mid: " << (quantiles.size() > 1 ? quantiles[1] : INVALID) << "|"
                                      << "max: " << (quantiles.size() > 2 ? quantiles[2] : INVALID) << "|"
                                      << std::endl;
                        }
                    }
                }
                else
                {
                    obj.AddMember(rapidjson::Value().SetString(metric.outputName.c_str(), metric.outputName.length(), allocator)
                                  , rapidjson::Value().SetDouble(metric.value), allocator);
                }
            }

            if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == ctx.m_glogLevel)
            {
                LOG(INFO) << "rule_id: " << key.ruleId << "|"
                          << "timestamp: " << key.timeStamp * 1000 << "|"
                          << "eventCount: " << eventCount << "|"
                          << "value: " << value << "|"
                          << std::endl;
            }

            // 一个 AggregatorValue 表示一条数据
            doc.PushBack(obj, allocator);

            ++iter;
        }

        reportToServer(doc);

        return ret;
    }

    void ReportThread::reportStat(MonitorContext &ctx)
    {
        int ruleId(ctx.getSelfMonitorId());
        long lostByAggregate(ctx.m_statInfo.lostByAggregate);                     // 维度基数过大超过限制后丢弃的数量
        ctx.m_statInfo.lostByAggregate = 0;
        long disruptProcessFail(ctx.m_statInfo.disruptProcessFail);               // 数据处理异常的数量
        ctx.m_statInfo.disruptProcessFail = 0;
        long dimensionCount(ctx.m_statInfo.dimensionCount);                       // ruleID的维度基数
        ctx.m_statInfo.dimensionCount = 0;
        long beesSendFailCount(ctx.m_statInfo.beesSendFailCount);                 // bees发送失败的次数/数据条数
        ctx.m_statInfo.beesSendFailCount = 0;
        long flumeSendFailCount(ctx.m_statInfo.flumeSendFailCount);               // flume发送失败的次数/数据条数
        ctx.m_statInfo.flumeSendFailCount = 0;
        long mapSize(ctx.m_statInfo.mapSize);                                     // SDK中缓存的维度基数大小
        ctx.m_statInfo.mapSize = 0;
        long addDisruptSuccess(ctx.m_statInfo.addDisruptSuccess);                 // 放入队列成功的数量
        ctx.m_statInfo.addDisruptSuccess = 0;
        long addDisruptFail(ctx.m_statInfo.addDisruptFail);                       // 放入队列失败的数量
        ctx.m_statInfo.addDisruptFail = 0;
        long directSendSuccessSize(ctx.m_statInfo.directSendSuccessSize);         // 数据直接发送成功的条数
        ctx.m_statInfo.directSendSuccessSize = 0;
        long sendMaxTimeByBees(ctx.m_statInfo.sendMaxTimeByBees);                 // bees 传输最大耗时
        ctx.m_statInfo.sendMaxTimeByBees = 0;
        long sendMaxTimeByFlume(ctx.m_statInfo.sendMaxTimeByFlume);               // flume 传输最大耗时
        ctx.m_statInfo.sendMaxTimeByFlume = 0;

        int ret(com::vivo::bees::bus::thrift::Status::UNKNOWN);
        if (0 == lostByAggregate && 0 == disruptProcessFail && 0 == dimensionCount
            && 0 == beesSendFailCount && 0 == flumeSendFailCount && 0 == mapSize
            && 0 == addDisruptSuccess && 0 == addDisruptFail && 0 == directSendSuccessSize
            && 0 == sendMaxTimeByBees && 0 == sendMaxTimeByFlume)
        {
            return;
        }

        rapidjson::Document doc;
        doc.SetArray();
        rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();

        rapidjson::Value obj(rapidjson::kObjectType);
        obj.AddMember("rule_id", ruleId, allocator);
        std::chrono::milliseconds now = NOW_MILLI_SECONDS;
        const long &timeStamp(now.count());
        obj.AddMember("timestamp", timeStamp, allocator);
        obj.AddMember("lostByAggregate", lostByAggregate, allocator);
        obj.AddMember("disruptProcessFail", disruptProcessFail, allocator);
        obj.AddMember("dimensionCount", dimensionCount, allocator);
        obj.AddMember("beesSendFailCount", beesSendFailCount, allocator);
        obj.AddMember("flumeSendFailCount", flumeSendFailCount, allocator);
        obj.AddMember("mapSize", mapSize, allocator);
        obj.AddMember("addDisruptSuccess", addDisruptSuccess, allocator);
        obj.AddMember("addDisruptFail", addDisruptFail, allocator);
        obj.AddMember("directSendSuccessSize", directSendSuccessSize, allocator);
        obj.AddMember("sendMaxTimeByBees", sendMaxTimeByBees, allocator);
        obj.AddMember("sendMaxTimeByFlume", sendMaxTimeByFlume, allocator);
        obj.AddMember("IP", rapidjson::Value().SetString(ctx.m_serverInfo.ip.c_str(), ctx.m_serverInfo.ip.length(), allocator), allocator);
        obj.AddMember("hostname", rapidjson::Value().SetString(ctx.m_serverInfo.host.c_str(), ctx.m_serverInfo.host.length(), allocator), allocator);
        obj.AddMember("appCode", rapidjson::Value().SetString(ctx.m_appCode.c_str(), ctx.m_appCode.length(), allocator), allocator);
        std::string beesCfg(ctx.getEnv());
        obj.AddMember("env", rapidjson::Value().SetString(beesCfg.c_str(), beesCfg.length(), allocator), allocator);
        obj.AddMember("initRuleIdArray", rapidjson::Value().SetString(ctx.m_ruleIds.c_str(), ctx.m_ruleIds.length(), allocator), allocator);
        obj.AddMember("isFlumeSend", ctx.getFlumeSend(), allocator);

        // idc信息暂时没有，咨询贺炎后确认暂时可不填
        //obj.AddMember("idc", "idc", allocator);

        doc.PushBack(obj, allocator);

        rapidjson::StringBuffer strBuf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
        doc.Accept(writer);
        vivo_bees_bus_cpp_report::BeesBusThriftFlumeEvent event;
        ctx.getHeaders(event.headers);
        const std::string &body(strBuf.GetString());
        event.__set_body(body);

        if (!event.body.empty())
        {
            ret = vivo_bees_bus_cpp_report::BeesBusReport::append(vivo_bees_bus_cpp_report::AppendContext{ctx.getBeesTaskName()}, event);
        }
        else
        {
            LOG(ERROR) << "report failed! rule_id: " << ruleId << "|"
                       << "timestamp: " << timeStamp << "|"
                       << "body: " << body << "|"
                       << "append ret: " << ret << "|"
                       << std::endl;
        }

        if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == ctx.m_glogLevel)
        {
            LOG(INFO) << "rule_id: " << ruleId << "|"
                      << "timestamp: " << timeStamp << "|"
                      << "body: " << body << "|"
                      << "append ret: " << ret << "|"
                      << std::endl;
        }
    }

    void ReportThread::getReportAggregatorMap(MonitorContext &ctx, AggregatorMap &destAggregatorMap)
    {
        auto iter = ctx.m_bufferAggregatorMap.begin();
        // 将满足条件的数据取出上报并从原始缓存中删除
        while (ctx.m_bufferAggregatorMap.end() != iter)
        {
            if (ctx.isRuleIdReport(iter->first.ruleId, iter->first.timeStamp))
            {
                destAggregatorMap.emplace_hint(destAggregatorMap.end(), iter->first, iter->second);
                ctx.m_bufferAggregatorMap.erase(iter++);

                continue;
            }

            ++iter;
        }
    }
}