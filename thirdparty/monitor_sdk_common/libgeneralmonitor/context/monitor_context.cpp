//
// Created by 杨民善 on 2020/6/22.
//

#include "../util/util.h"
#include "monitor_context.h"

#include <sstream>
#include <cmath>

#include "thirdparty/rapidjson/document.h"
#include "thirdparty/rapidjson/stringbuffer.h"
#include "thirdparty/rapidjson/writer.h"
#include "../../libbeesbus/log/glog_helper.h"

namespace vivo_general_monitor_cpp_report
{
    LockFreeQueue *G_P_LOCK_FREE_QUEUE = NULL;                     // 无锁原始数据缓存队列
    LockFreeBufferQueue *G_P_LOCK_FREE_BUFFER_QUEUE = NULL;        // 无锁不聚合缓存队列

    static bool dimensionCompare(const std::pair<std::string, std::string> &l, const std::pair<std::string, std::string> &r)
    {
        return l.first < r.first;
    }

    int MonitorContext::init(const MonitorInitInfo &info)
    {
        m_monitorInitInfo = info;

        return MONITOR_CPP_REPORT_SUCCESS;
    }

    std::mutex* MonitorContext::getMutexByThreadId(const std::thread::id &threadId)
    {
        std::map<std::thread::id, std::mutex*>::iterator iter = m_mapMutex.find(threadId);

        return m_mapMutex.end() != iter ? iter->second : NULL;
    }

    void MonitorContext::handleMapStat(const QueueData &queueData, const std::thread::id &threadId)
    {
        RuleConfig ruleConfig;
        bool isGetSuccess(getRuleConfig(ruleConfig, queueData.ruleId));
        if (isGetSuccess && COLLECT_STATUS_ON == ruleConfig.collectStatus)
        {
            const ReportMap &mapStat = queueData.mapStat;
            AggregatorKey aggregatorKey;
            aggregatorKey.ruleId = queueData.ruleId;
            aggregatorKey.timeStamp = getCollectTime(queueData.timeStamp, ruleConfig.granularity);
            AggregatorValue aggregatorValue;
            aggregatorValue.count += 1;
            std::vector<std::pair<std::string, std::string>> &vtDimension = aggregatorKey.vtDimension;
            std::vector<std::pair<std::string, Metric>> &vtMetric = aggregatorValue.vtMetric;
            // 维度校验
            const std::vector<DimensionInfo> &vtDimensionInfo = ruleConfig.vtDimensionInfo;
            const size_t DIMENSION_SIZE(vtDimensionInfo.size());
            for (size_t i = 0; i < DIMENSION_SIZE; ++i)
            {
                const DimensionInfo &info = vtDimensionInfo[i];
                ReportMapConstIterator iter = mapStat.find(info.name);
                if (mapStat.end() != iter)
                {
                    vtDimension.emplace_back(std::pair<std::string, std::string>(iter->first, iter->second));

                    if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == m_glogLevel)
                    {
                        LOG(INFO) << "threadId: " << threadId << "|"
                                  << "rule id: " << aggregatorKey.ruleId << "|"
                                  << "timeStamp: " << queueData.timeStamp << "|"
                                  << "collect timeStamp: " << aggregatorKey.timeStamp << "|"
                                  << iter->first <<  ": " << iter->second << "|"
                                  << std::endl;
                    }
                }
            }

            std::sort(vtDimension.begin(), vtDimension.end(), dimensionCompare);

            // 指标校验
            const std::vector<MetricInfo> &vtMetricInfo = ruleConfig.vtMetricInfo;
            const size_t METRIC_SIZE(vtMetricInfo.size());
            for (size_t i = 0; i < METRIC_SIZE; ++i)
            {
                const MetricInfo &info = vtMetricInfo[i];
                ReportMapConstIterator iter = mapStat.find(info.name);
                if (mapStat.end() != iter)
                {
                    Metric metric;
                    metric.name = info.name;
                    metric.outputName = info.outputName;
                    metric.aggregator = info.aggregator;
                    metric.k = info.k;
                    metric.vtFractions = info.vtFractions;
                    metric.vtFractionName = info.vtFractionName;

                    try
                    {
                        metric.value = std::stod(iter->second);
                        auto pos = iter->second.find(".");
                        if (std::string::npos != pos)
                        {
                            metric.precision = static_cast<int>(iter->second.length() - pos) - 1;
                        }
                    }
                    catch (std::exception &ex)
                    {
                        LOG(ERROR) << "rule id: " << aggregatorKey.ruleId << "|"
                                   << "timeStamp: " << queueData.timeStamp << "|"
                                   << "collect timeStamp: " << aggregatorKey.timeStamp << "|"
                                   << "metric name: " << metric.name << "|"
                                   << "metric outputName: " << metric.outputName << "|"
                                   << "metric aggregator: " << metric.aggregator << "|"
                                   << "metric value: " << metric.value << "|"
                                   << "metric precision: " << metric.precision << "|"
                                   << "stod exception: " << ex.what() << "|"
                                   << std::endl;
                    }
                    catch (...)
                    {
                        LOG(ERROR) << "rule id: " << aggregatorKey.ruleId << "|"
                                   << "timeStamp: " << queueData.timeStamp << "|"
                                   << "collect timeStamp: " << aggregatorKey.timeStamp << "|"
                                   << "metric name: " << metric.name << "|"
                                   << "metric outputName: " << metric.outputName << "|"
                                   << "metric aggregator: " << metric.aggregator << "|"
                                   << "metric value: " << metric.value << "|"
                                   << "metric precision: " << metric.precision << "|"
                                   << "stod unknown exception!"
                                   << std::endl;
                    }

                    if (metric.k > 0 && AGGREGATOR_QUANTILE == metric.aggregator)
                    {
                        DataSketch sketch(metric.k);
                        sketch.update(metric.value);
                        metric.sketch = std::move(sketch);
                    }

                    vtMetric.emplace_back(std::pair<std::string, Metric>(info.outputName, metric));

                    if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == m_glogLevel)
                    {
                        LOG(INFO) << "rule id: " << aggregatorKey.ruleId << "|"
                                  << "timeStamp: " << queueData.timeStamp << "|"
                                  << "collect timeStamp: " << aggregatorKey.timeStamp << "|"
                                  << "metric name: " << metric.name << "|"
                                  << "metric outputName: " << metric.outputName << "|"
                                  << "metric aggregator: " << metric.aggregator << "|"
                                  << "metric value: " << metric.value << "|"
                                  << "metric precision: " << metric.precision << "|"
                                  << std::endl;
                    }
                }
                else
                {
                    if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == m_glogLevel)
                    {
                        LOG(INFO) << "rule id: " << aggregatorKey.ruleId << "|"
                                  << "timeStamp: " << queueData.timeStamp << "|"
                                  << "collect timeStamp: " << aggregatorKey.timeStamp << "|"
                                  << "metric name: " << info.name << "|not find!|"
                                  << std::endl;
                    }
                }
            }

            if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == m_glogLevel)
            {
                LOG(INFO) << "rule id: " << aggregatorKey.ruleId << "|"
                          << "timeStamp: " << queueData.timeStamp << "|"
                          << "collect timeStamp: " << aggregatorKey.timeStamp << "|"
                          << "rollupStatus: " << ruleConfig.rollupStatus << "|"
                          << "aggregatorValue: " << aggregatorValue << "|"
                          << std::endl;
            }

            // 无论维度、指标列表是否为空都要处理：1、非空则按正常逻辑聚合或上报；2、为空则聚合时count计数仍然要+1；
            if (ROLLUP_STATUS_ON == ruleConfig.rollupStatus) // 需要聚合
            {
                mergeAggregatorValue(aggregatorKey, aggregatorValue, threadId);
            }
            else if (ROLLUP_STATUS_OFF == ruleConfig.rollupStatus) // 直接上报
            {
                //report(aggregatorKey, aggregatorValue);
                if (!insertBufferData(aggregatorKey, aggregatorValue))
                {
                    ++m_statInfo.disruptProcessFail;
                    LOG(ERROR) << "insertBufferData failed aggregatorKey: " << aggregatorKey << "|"
                               << std::endl;
                }
            }
        }
        else
        {
            LOG(ERROR) << "rule id: " << queueData.ruleId
                       << (!isGetSuccess ? " invalid!" : (COLLECT_STATUS_OFF == ruleConfig.collectStatus ? " COLLECT_STATUS_OFF" : " unknown error")) << "|"
                       << std::endl;
        }
    }

    void MonitorContext::mergeAggregatorValue(AggregatorKey &key, AggregatorValue &value, const std::thread::id &threadId)
    {
        // 与上报线程作互斥处理
        std::mutex* pMutex(getMutexByThreadId(threadId));
        if (pMutex)
        {
            std::map<std::thread::id, AggregatorMap>::iterator iter = m_mapAggregator.find(threadId);
            if (m_mapAggregator.end() != iter)
            {
                std::lock_guard<std::mutex> guard(*pMutex);

                mergeAggregatorMap(key, value, iter->second);
            }
            else
            {
                ++m_statInfo.disruptProcessFail;

                LOG(ERROR) << "threadId: " << threadId << "|"
                           << "insert first value!" << "|"
                           << "rule id: " << key.ruleId << "|"
                           << "collect timeStamp: " << key.timeStamp << "|"
                           << "value: " << value << "|"
                           << "lost!|"
                           << std::endl;
            }
        }
        else
        {
            ++m_statInfo.disruptProcessFail;
            LOG(ERROR) << "threadId: " << threadId << "|"
                       << "get mutex error!" << "|"
                       << "rule id: " << key.ruleId << "|"
                       << "collect timeStamp: " << key.timeStamp << "|"
                       << "value: " << value << "|"
                       << "lost!" << "|"
                       << std::endl;
        }
    }

    void MonitorContext::mergeAggregatorMap(const AggregatorKey &key, AggregatorValue &value, AggregatorMap &aggregatorMap, bool isLimit)
    {
        auto aggregatorIter = aggregatorMap.find(key);
        if (aggregatorMap.end() != aggregatorIter)
        {
            AggregatorValue &aggregatorValue = aggregatorIter->second;
            aggregatorValue.count += value.count;
            mergeMetric(aggregatorValue.vtMetric, value.vtMetric);
            if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == m_glogLevel)
            {
                LOG(INFO) << "rule id: " << key.ruleId << "|"
                          << "collect timeStamp: " << key.timeStamp << "|"
                          << "merge|source value: " << value << "|"
                          << "dest value: " << aggregatorValue << "|"
                          << std::endl;
            }
        }
        else
        {
            if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == m_glogLevel)
            {
                LOG(INFO) << "rule id: " << key.ruleId << "|"
                          << "collect timeStamp: " << key.timeStamp << "|"
                          << "merge|new value: " << value << "|"
                          << std::endl;
            }

            const auto MAX_MAP_SIZE(static_cast<size_t>(getMapMaxSize()));
            const size_t SIZE(aggregatorMap.size());
            if (MAX_MAP_SIZE > SIZE)
            {
                aggregatorMap[key] = std::move(value);
            }
            else
            {
                ++m_statInfo.lostByAggregate;

                LOG(ERROR) << "rule_id: " << key.ruleId << "|"
                           << "timestamp: " << key.timeStamp << "|"
                           << "aggregatorMap size: " << SIZE << "|"
                           << "mapMaxSize: " << MAX_MAP_SIZE << "|"
                           << "value: " << value << "|"
                           << "lostByAggregate!" << "|"
                           << std::endl;
            }
        }
    }

    void MonitorContext::mergeMetric(std::vector<std::pair<std::string, Metric>> &vtDest, std::vector<std::pair<std::string, Metric>> &vtSource, bool isCommonCalc)
    {
        const size_t SOURCE_SIZE(vtSource.size());
        for (size_t i = 0; i < SOURCE_SIZE; ++i)
        {
            auto &source = vtSource[i];
            std::vector<std::pair<std::string, Metric>>::iterator iter = std::find(vtDest.begin(), vtDest.end(), source);
            if (vtDest.end() != iter)
            {
                const auto SOURCE_VALUE(source.second.value);
                // 求和
                if (AGGREGATOR_SUM == iter->second.aggregator)
                {
                    iter->second.value += SOURCE_VALUE;
                }
                else if (AGGREGATOR_MAX == iter->second.aggregator) // 取最大值
                {
                    iter->second.value = (std::isgreater(SOURCE_VALUE, iter->second.value) ? SOURCE_VALUE : iter->second.value);
                }
                else if (AGGREGATOR_MIN == iter->second.aggregator) // 取最小值
                {
                    iter->second.value = (std::isless(SOURCE_VALUE, iter->second.value) ? SOURCE_VALUE : iter->second.value);
                }
                else if (AGGREGATOR_QUANTILE == iter->second.aggregator) // 分位
                {
                    iter->second.sketch.merge(source.second.sketch);
                }
            }
            else // 新指标
            {
                vtDest.emplace_back(source);
            }
        }
    }

    void MonitorContext::getHeaders(std::map<std::string, std::string> &headers)
    {
        size_t index(getBeesReadIndex());
        const BeesConfig &config = m_beesConfig[index];
        headers.insert(std::pair<std::string, std::string>("appName", config.beesTaskName));
        std::chrono::milliseconds ms = NOW_MILLI_SECONDS;
        std::ostringstream buf;
        buf << ms.count();
        headers.insert(std::pair<std::string, std::string>("data_timestamp", buf.str()));
        headers.insert(std::pair<std::string, std::string>("topic", config.topic));
        headers.insert(std::pair<std::string, std::string>("lines", "1"));

        // 测试环境 flume 参数
        headers.insert(std::pair<std::string, std::string>("project", "monitor-sdk"));

        if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == m_glogLevel)
        {
            LOG(INFO) << "appName: " << config.appName << "|"
                      << "data_timestamp: " << ms.count() << "|"
                      << "topic: " << config.topic << "|"
                      << "project: " << "monitor-sdk" << "|"
                      << std::endl;
        }
    }

    int MonitorContext::report(AggregatorKey &aggregatorKey, AggregatorValue &aggregatorValue)
    {
        int ret(com::vivo::bees::bus::thrift::Status::UNKNOWN);
        rapidjson::Document doc;
        doc.SetArray();
        rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();

        rapidjson::Value obj(rapidjson::kObjectType);
        obj.AddMember("rule_id", aggregatorKey.ruleId, allocator);
        obj.AddMember("timestamp", aggregatorKey.timeStamp * 1000, allocator);
        AggregatorValue &value = aggregatorValue;
        const size_t &eventCount(value.count > 0 ? value.count - 1 : 0);
        obj.AddMember("eventCount", eventCount, allocator);
        // 维度
        std::vector<std::pair<std::string, std::string>> &vtDimension = aggregatorKey.vtDimension;
        const size_t DIMEN_SIZE(vtDimension.size());
        for (size_t j = 0; j < DIMEN_SIZE; ++j)
        {
            obj.AddMember(rapidjson::Value().SetString(vtDimension[j].first.c_str(), vtDimension[j].first.length(), allocator)
                          , rapidjson::Value().SetString(vtDimension[j].second.c_str(), vtDimension[j].second.length(), allocator)
                          , allocator);
        }

        // 指标
        const std::vector<std::pair<std::string, Metric>> &vtMetric = value.vtMetric;
        const size_t METRIC_SIZE(vtMetric.size());
        for (size_t k = 0; k < METRIC_SIZE; ++k)
        {
            obj.AddMember(rapidjson::Value().SetString(vtMetric[k].second.outputName.c_str(), vtMetric[k].second.outputName.length(), allocator)
                          , rapidjson::Value().SetInt(vtMetric[k].second.value), allocator);
        }

        doc.PushBack(obj, allocator);

        rapidjson::StringBuffer strBuf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
        doc.Accept(writer);
        vivo_bees_bus_cpp_report::BeesBusThriftFlumeEvent event;
        getHeaders(event.headers);
        const std::string &body(strBuf.GetString());
        event.__set_body(body);

        if (!event.body.empty())
        {
            std::chrono::milliseconds now = NOW_MILLI_SECONDS;
            long begin(now.count());
            ret = vivo_bees_bus_cpp_report::BeesBusReport::append(vivo_bees_bus_cpp_report::AppendContext{getBeesTaskName()}, event);
            now = NOW_MILLI_SECONDS;
            long diff(now.count() - begin);

            if (0 != ret)
            {
                switch (getFlumeSend())
                {
                    case 0:
                        ++m_statInfo.beesSendFailCount;
                        break;
                    case 1:
                        ++m_statInfo.flumeSendFailCount;
                        break;
                    default:
                        break;
                }
            }
            else
            {
                switch (getFlumeSend())
                {
                    case 0:
                        if (diff > m_statInfo.sendMaxTimeByBees)
                        {
                            m_statInfo.sendMaxTimeByBees = diff;
                        }
                        break;
                    case 1:
                        if (diff > m_statInfo.sendMaxTimeByBees)
                        {
                            m_statInfo.sendMaxTimeByFlume = diff;
                        }
                        break;
                    default:
                        break;
                }

                ++m_statInfo.directSendSuccessSize;
            }
        }
        else
        {
            ++m_statInfo.disruptProcessFail;

            LOG(ERROR) << "report failed! rule_id: " << aggregatorKey.ruleId << "|"
                       << "timestamp: " << aggregatorKey.timeStamp * PER_SECOND_MILLI << "|"
                       << "eventCount: " << eventCount << "|"
                       << "value: " << value << "|"
                       << "body: " << body << "|"
                       << "append ret: " << ret << "|"
                       << std::endl;
        }

        if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == m_glogLevel)
        {
            LOG(INFO) << "rule_id: " << aggregatorKey.ruleId << "|"
                      << "timestamp: " << aggregatorKey.timeStamp * PER_SECOND_MILLI << "|"
                      << "eventCount: " << value.count - 1 << "|"
                      << "value: " << value << "|"
                      << "body: " << body << "|"
                      << "append ret: " << ret << "|"
                      << std::endl;
        }

        return ret;
    }

    int MonitorContext::report(std::vector<BufferData> &vtBufferData)
    {
        if (vtBufferData.empty())
        {
            return MONITOR_CPP_REPORT_DATA_EMPTY;
        }

        int ret(com::vivo::bees::bus::thrift::Status::UNKNOWN);
        rapidjson::Document doc;
        doc.SetArray();
        rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();

        const size_t SIZE(vtBufferData.size());
        for (size_t i = 0; i < SIZE; ++i)
        {
            AggregatorKey &aggregatorKey(vtBufferData[i].key);
            AggregatorValue &aggregatorValue(vtBufferData[i].value);
            rapidjson::Value obj(rapidjson::kObjectType);
            obj.AddMember("rule_id", aggregatorKey.ruleId, allocator);
            obj.AddMember("timestamp", aggregatorKey.timeStamp * PER_SECOND_MILLI, allocator);
            AggregatorValue &value = aggregatorValue;
            const size_t &eventCount(value.count > 0 ? value.count - 1 : 0);
            obj.AddMember("eventCount", eventCount, allocator);
            // 维度
            std::vector<std::pair<std::string, std::string>> &vtDimension = aggregatorKey.vtDimension;
            const size_t DIMEN_SIZE(vtDimension.size());
            for (size_t j = 0; j < DIMEN_SIZE; ++j)
            {
                obj.AddMember(rapidjson::Value().SetString(vtDimension[j].first.c_str(), vtDimension[j].first.length(), allocator)
                        , rapidjson::Value().SetString(vtDimension[j].second.c_str(), vtDimension[j].second.length(), allocator)
                        , allocator);
            }

            // 指标
            const std::vector<std::pair<std::string, Metric>> &vtMetric = value.vtMetric;
            const size_t METRIC_SIZE(vtMetric.size());
            for (size_t k = 0; k < METRIC_SIZE; ++k)
            {
                obj.AddMember(rapidjson::Value().SetString(vtMetric[k].second.outputName.c_str(), vtMetric[k].second.outputName.length(), allocator)
                              , rapidjson::Value().SetDouble(vtMetric[k].second.value), allocator);
            }

            doc.PushBack(obj, allocator);
        }

        rapidjson::StringBuffer strBuf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
        doc.Accept(writer);
        vivo_bees_bus_cpp_report::BeesBusThriftFlumeEvent event;
        getHeaders(event.headers);
        const std::string &body(strBuf.GetString());
        event.__set_body(body);

        long diff(0);
        if (!event.body.empty())
        {
            std::chrono::milliseconds now = NOW_MILLI_SECONDS;
            long begin(now.count());
            ret = vivo_bees_bus_cpp_report::BeesBusReport::append(vivo_bees_bus_cpp_report::AppendContext{getBeesTaskName()}, event);
            now = NOW_MILLI_SECONDS;
            diff = now.count() - begin;

            if (0 != ret)
            {
                switch (getFlumeSend())
                {
                    case 0:
                        ++m_statInfo.beesSendFailCount;
                        break;
                    case 1:
                        ++m_statInfo.flumeSendFailCount;
                        break;
                    default:
                        break;
                }
            }
            else
            {
                switch (getFlumeSend())
                {
                    case 0:
                        if (diff > m_statInfo.sendMaxTimeByBees)
                        {
                            m_statInfo.sendMaxTimeByBees = diff;
                        }
                        break;
                    case 1:
                        if (diff > m_statInfo.sendMaxTimeByBees)
                        {
                            m_statInfo.sendMaxTimeByFlume = diff;
                        }
                        break;
                    default:
                        break;
                }

                ++m_statInfo.directSendSuccessSize;
            }
        }
        else
        {
            ++m_statInfo.disruptProcessFail;

            LOG(ERROR) << "vtBufferData size: " << SIZE << "|"
                       << "event body empty!" << std::endl;
        }

        if (vivo_bees_bus_cpp_report::VIVO_BEES_GLOG_LEVEL_INFO == m_glogLevel)
        {
            LOG(INFO) << "vtBufferData size: " << SIZE << "|"
                      << "append cost: " << diff << "|"
                      << "body: " << body << "|"
                      << "append ret: " << ret << "|"
                      << std::endl;
        }

        return ret;
    }

    bool MonitorContext::insertBufferData(AggregatorKey &aggregatorKey, AggregatorValue &aggregatorValue)
    {
        bool insertResult(false);
        if (G_P_LOCK_FREE_BUFFER_QUEUE)
        {
            auto *pBuffer = new BufferData(aggregatorKey, aggregatorValue);
            if (nullptr != pBuffer)
            {
                insertResult = G_P_LOCK_FREE_BUFFER_QUEUE->bounded_push(pBuffer);
                if (!insertResult)
                {
                    LOG(ERROR) << "delete not insert buffer data: " << aggregatorKey << "|"
                               << "aggregatorValue: " << pBuffer->value << "|"
                               << std::endl;
                    delete pBuffer;
                    pBuffer = nullptr;
                }
            }
        }
        else
        {
            LOG(ERROR) << "lock free queue G_P_LOCK_FREE_BUFFER_QUEUE is NULL!" << std::endl;
        }

        return insertResult;
    }
}