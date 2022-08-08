//
// Created by 杨民善 on 2020/6/22.
//

#include "../util/util.h"
#include "monitor_config.h"

#include <unistd.h>
#include <sstream>

#include "thirdparty/rapidjson/document.h"
#include "thirdparty/rapidjson/stringbuffer.h"
#include "thirdparty/rapidjson/writer.h"
#include "thirdparty/glog/logging.h"

namespace vivo_general_monitor_cpp_report
{
    void MonitorConfig::monitorConfigTask(MonitorContext &ctx)
    {
        long lastGetTime(0);
        unsigned int loopInterval = ctx.m_loopInterval;
        while (!ctx.m_isTerminate)
        {
            std::chrono::seconds now = NOW_SECONDS;
            if (now.count() - lastGetTime > loopInterval)
            {
                lastGetTime = now.count();
                if (MONITOR_CPP_REPORT_SUCCESS == getMonitorConfig(ctx))
                {
                    loopInterval = ctx.m_loopInterval;
                }
                else
                {
                    loopInterval = QUICK_INTERVAL;
                }
            }

            std::this_thread::sleep_for(std::chrono::seconds(DEFAULT_SECOND));
        }
    }

    int MonitorConfig::getMonitorConfig(MonitorContext &ctx)
    {
        const std::string &url = (ctx.m_monitorInitInfo.configUrl);
        if (url.empty())
        {
            LOG(ERROR) << "get monitor config list url empty!" << url << "|"
                       << std::endl;

            return MONITOR_CPP_REPORT_CONFIG_URL_EMPTY;
        }

        rapidjson::Document doc;
        doc.SetObject();
        rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
        rapidjson::Value IntArray(rapidjson::kArrayType);
        for (size_t i = 0; i < ctx.m_monitorInitInfo.vtRuleId.size(); ++i)
        {
            IntArray.PushBack(ctx.m_monitorInitInfo.vtRuleId[i], allocator);
        }
        doc.AddMember("ruleIds", IntArray, allocator);
        doc.AddMember("hostIp", rapidjson::Value().SetString(ctx.m_serverInfo.ip.c_str(), ctx.m_serverInfo.ip.length()), allocator);
        doc.AddMember("clientType", rapidjson::Value().SetString(MONITOR_LANGUAGE_TYPE.c_str(), MONITOR_LANGUAGE_TYPE.length()), allocator);
        doc.AddMember("appCode", rapidjson::Value().SetString(ctx.m_appCode.c_str(), ctx.m_appCode.length()), allocator);

        rapidjson::StringBuffer strBuf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
        doc.Accept(writer);
        common::HttpResponse response;
        common::HttpClient::Options options;
        options.AddHeader("Content-Type", "application/json;charset=UTF-8");
        //options.AddHeader("Accept", "application/json");
        options.AddHeader("spaceId", ctx.m_monitorInitInfo.spaceId);
        std::chrono::milliseconds ms = NOW_MILLI_SECONDS;
        std::ostringstream buf;
        buf << ms.count();
        options.AddHeader("timestamp", buf.str());
        std::string md5(md5str(ctx.m_monitorInitInfo.spaceId + buf.str() + ctx.m_monitorInitInfo.token));
        buf.str("");
        buf << ms.count() - 60 * 1000;
        options.AddHeader("lastScheduledTimeStamp", "28800000");
        options.AddHeader("md5", md5);
        common::HttpClient::ErrorCode error;
        std::string data = strBuf.GetString();
        ctx.m_client.Post(url, data, options, &response, &error);

        const int status{response.Status()};
        if (common::HttpClient::SUCCESS == error && common::HttpResponse::Status_OK == status)
        {
            LOG(INFO) << "url: " << url << "|"
                      << "token: " << ctx.m_monitorInitInfo.token << "|"
                      << "spaceId: " << ctx.m_monitorInitInfo.spaceId << "|"
                      << "timestamp: " << ms.count() << "|"
                      << "md5: " << md5 << "|"
                      << "request data: " << data << "|"
                      << "http status: " << status << "|"
                      << "error: " << error << "|"
                      << std::endl;

            return handleMonitorConfigResponse(ctx, response.Body());
        }
        else
        {
            LOG(ERROR) << "get monitor config list failed url: " << url << "|"
                       << "token: " << ctx.m_monitorInitInfo.token << "|"
                       << "spaceId: " << ctx.m_monitorInitInfo.spaceId << "|"
                       << "timestamp: " << ms.count() << "|"
                       << "md5: " << md5 << "|"
                       << "request data: " << data << "|"
                       << "http status: " << status << "|"
                       << "error: " << error << "|"
                       << std::endl;
        }

        return MONITOR_CPP_REPORT_CONFIG_FAILED;
    }

    int MonitorConfig::handleMonitorConfigResponse(MonitorContext &ctx, const std::string &jsonStr)
    {
        rapidjson::Document doc;
        if (!doc.Parse(jsonStr.data()).HasParseError())
        {
            std::ostringstream buf;
            buf << "jsonStr: " << jsonStr << "|";
            int code(DEFAULT_ERROR_CODE);
            if(doc.HasMember("code") && doc["code"].IsInt())
            {
                code = doc["code"].GetInt();
            }

            buf << "code: " << code << "|";
            if (0 == code)
            {
                if (doc.HasMember("data") && doc["data"].IsObject())
                {
                    const rapidjson::Value &data = doc["data"];
                    // 解析通用配置
                    CommonConfig commonConfig;
                    if (data.HasMember("commonConfig") && data["commonConfig"].IsObject())
                    {
                        const rapidjson::Value &comonCfg = data["commonConfig"];
                        if (comonCfg.HasMember("update") && comonCfg["update"].IsBool())
                        {
                            commonConfig.update = comonCfg["update"].GetBool();
                        }

                        // 如果未返回数据则直接返回无需处理
                        if (!commonConfig.update)
                        {
                            LOG(INFO) << buf.str() << std::endl;

                            return MONITOR_CPP_REPORT_SUCCESS;
                        }

                        if (comonCfg.HasMember("mapMaxSize") && comonCfg["mapMaxSize"].IsInt())
                        {
                            commonConfig.mapMaxSize = comonCfg["mapMaxSize"].GetInt();
                        }

                        if (comonCfg.HasMember("disruptorBufferSize") && comonCfg["disruptorBufferSize"].IsInt())
                        {
                            commonConfig.disruptorBufferSize = comonCfg["disruptorBufferSize"].GetInt();
                        }

                        if (comonCfg.HasMember("corePoolSize") && comonCfg["corePoolSize"].IsInt())
                        {
                            commonConfig.corePoolSize = comonCfg["corePoolSize"].GetInt();
                        }

                        if (comonCfg.HasMember("beesSendPeriodDefault") && comonCfg["beesSendPeriodDefault"].IsInt())
                        {
                            commonConfig.beesSendPeriodDefault = comonCfg["beesSendPeriodDefault"].GetInt();
                        }

                        if (comonCfg.HasMember("selfMonitorId") && comonCfg["selfMonitorId"].IsInt())
                        {
                            commonConfig.selfMonitorId = comonCfg["selfMonitorId"].GetInt();
                        }

                        if (comonCfg.HasMember("thriftSendTimeout") && comonCfg["thriftSendTimeout"].IsInt())
                        {
                            commonConfig.thriftSendTimeout = comonCfg["thriftSendTimeout"].GetInt();
                        }

                        if (comonCfg.HasMember("thriftReceiveTimeout") && comonCfg["thriftReceiveTimeout"].IsInt())
                        {
                            commonConfig.thriftReceiveTimeout = comonCfg["thriftReceiveTimeout"].GetInt();
                        }
                    }

                    size_t index = ctx.getCommonWriteIndex();
                    ctx.m_commonConfig[index] = commonConfig;
                    ctx.swapCommonIndex();

                    // 解析 flume 配置
                    FlumeConfig flumeConfig;
                    if (data.HasMember("flumeConfig") && data["flumeConfig"].IsObject())
                    {
                        const rapidjson::Value &flumeCfg = data["flumeConfig"];
                        if (flumeCfg.HasMember("isFlumeSend") && flumeCfg["isFlumeSend"].IsString())
                        {
                            const std::string &isFlumeSend(flumeCfg["isFlumeSend"].GetString());
                            flumeConfig.isFlumeSend = static_cast<int>(std::strtol(isFlumeSend.c_str(), nullptr, 10));
                        }

                        if (flumeCfg.HasMember("flumeServerIp") && flumeCfg["flumeServerIp"].IsString())
                        {
                            flumeConfig.flumeServerIp = flumeCfg["flumeServerIp"].GetString();
                        }

                        if (flumeCfg.HasMember("flumeServerPort") && flumeCfg["flumeServerPort"].IsInt())
                        {
                            flumeConfig.flumeServerPort = flumeCfg["flumeServerPort"].GetInt();
                        }
                    }

                    // 解析批量发送配置
                    if (data.HasMember("batchDequeConfig") && data["batchDequeConfig"].IsObject())
                    {
                        const rapidjson::Value &batchDequeConfig = data["batchDequeConfig"];
                        if (batchDequeConfig.HasMember("batchListSize") && batchDequeConfig["batchListSize"].IsInt())
                        {
                            size_t batchListSize(batchDequeConfig["batchListSize"].GetInt());
                            if (batchListSize > 0)
                            {
                                flumeConfig.batchDequeConfig.batchListSize = batchListSize;
                            }
                        }

                        if (batchDequeConfig.HasMember("batchSendPeriod") && batchDequeConfig["batchSendPeriod"].IsString())
                        {
                            size_t batchSendPeriod(batchDequeConfig["batchSendPeriod"].GetInt());
                            if (batchSendPeriod > 0)
                            {
                                flumeConfig.batchDequeConfig.batchSendPeriod = batchSendPeriod;
                            }
                        }
                    }

                    index = ctx.getFlumeWriteIndex();
                    ctx.m_FlumeConfig[index] = flumeConfig;
                    ctx.swapFlumeIndex();

                    // 根据远端配置动态设置上报方式
                    vivo_bees_bus_cpp_report::BeesBusReport::setReportMethod(flumeConfig.isFlumeSend);

                    // 解析 bees 配置
                    BeesConfig beesConfig;
                    if (data.HasMember("beesConfig") && data["beesConfig"].IsObject())
                    {
                        const rapidjson::Value &beesCfg = data["beesConfig"];
                        if (beesCfg.HasMember("env") && beesCfg["env"].IsString())
                        {
                            beesConfig.env = beesCfg["env"].GetString();
                        }

                        if (beesCfg.HasMember("managerUrl") && beesCfg["managerUrl"].IsString())
                        {
                            beesConfig.managerUrl = beesCfg["managerUrl"].GetString();
                        }

                        if (beesCfg.HasMember("appName") && beesCfg["appName"].IsString())
                        {
                            beesConfig.appName = beesCfg["appName"].GetString();
                        }

                        if (beesCfg.HasMember("topic") && beesCfg["topic"].IsString())
                        {
                            beesConfig.topic = beesCfg["topic"].GetString();
                        }

                        if (beesCfg.HasMember("beesTaskName") && beesCfg["beesTaskName"].IsString())
                        {
                            beesConfig.beesTaskName = beesCfg["beesTaskName"].GetString();
                        }

                        if (beesCfg.HasMember("beesThreadPollingPeriod") && beesCfg["beesThreadPollingPeriod"].IsInt())
                        {
                            int beesThreadPollingPeriod = beesCfg["beesThreadPollingPeriod"].GetInt();
                            if (beesThreadPollingPeriod > 0)
                            {
                                beesConfig.beesThreadPollingPeriod = static_cast<unsigned int>(beesThreadPollingPeriod);
                            }
                        }
                    }

                    index = ctx.getBeesWriteIndex();
                    ctx.m_beesConfig[index] = beesConfig;
                    ctx.swapBeesIndex();

                    // bees 配置更新后设置一下线程轮询间隔
                    vivo_bees_bus_cpp_report::BeesBusReport::setLoopInterval(ctx.getBeesLoopInterval());

                    // 解析 ruleConfig 配置
                    RuleMap mapRuleConfig;
                    if (data.HasMember("ruleConfig") && data["ruleConfig"].IsArray())
                    {
                        const rapidjson::Value &ruleCfgArray = data["ruleConfig"];
                        const size_t RULE_SIZE(ruleCfgArray.Size());
                        for (size_t i = 0; i < RULE_SIZE; ++i)
                        {
                            RuleConfig ruleConfig;
                            const rapidjson::Value &cfg = ruleCfgArray[i];
                            if (cfg.HasMember("ruleId") && cfg["ruleId"].IsInt())
                            {
                                ruleConfig.ruleId = cfg["ruleId"].GetInt();
                            }

                            if (0 == ruleConfig.ruleId)
                            {
                                continue;
                            }

                            if (cfg.HasMember("granularity") && cfg["granularity"].IsInt())
                            {
                                ruleConfig.granularity = cfg["granularity"].GetInt();
                            }

                            if (cfg.HasMember("collectStatus") && cfg["collectStatus"].IsInt())
                            {
                                ruleConfig.collectStatus = cfg["collectStatus"].GetInt();
                            }

                            if (cfg.HasMember("rollupStatus") && cfg["rollupStatus"].IsInt())
                            {
                                ruleConfig.rollupStatus = cfg["rollupStatus"].GetInt();
                            }

                            if (cfg.HasMember("earlyMessageRejectionPeriod") && cfg["earlyMessageRejectionPeriod"].IsInt())
                            {
                                auto period(cfg["earlyMessageRejectionPeriod"].GetInt());
                                ruleConfig.earlyMessageRejectionPeriod = period > 0 ? period : 0;
                            }

                            if (cfg.HasMember("lateMessageRejectionPeriod") && cfg["lateMessageRejectionPeriod"].IsInt())
                            {
                                auto period(cfg["lateMessageRejectionPeriod"].GetInt());
                                ruleConfig.lateMessageRejectionPeriod = period > 0 ? period : 0;
                            }

                            if (cfg.HasMember("dimensionList") && cfg["dimensionList"].IsArray())
                            {
                                const rapidjson::Value &dimensionArray = cfg["dimensionList"];
                                const size_t DIMEN_SIZE(dimensionArray.Size());
                                for (size_t j = 0; j < DIMEN_SIZE; ++j)
                                {
                                    DimensionInfo dimensionInfo;
                                    const rapidjson::Value &dimensionCfg = dimensionArray[j];
                                    if (dimensionCfg.HasMember("id") && dimensionCfg["id"].IsInt())
                                    {
                                        dimensionInfo.id = dimensionCfg["id"].GetInt();
                                    }

                                    if (dimensionCfg.HasMember("name") && dimensionCfg["name"].IsString())
                                    {
                                        dimensionInfo.name = dimensionCfg["name"].GetString();
                                        ruleConfig.vtDimensionInfo.emplace_back(std::move(dimensionInfo));
                                    }
                                }
                            }

                            if (cfg.HasMember("metricList") && cfg["metricList"].IsArray())
                            {
                                const rapidjson::Value &metricArray = cfg["metricList"];
                                const size_t METRIC_SIZE(metricArray.Size());
                                for (size_t j = 0; j < METRIC_SIZE; ++j)
                                {
                                    MetricInfo metricInfo;
                                    const rapidjson::Value &metricCfg = metricArray[j];
                                    if (metricCfg.HasMember("id") && metricCfg["id"].IsInt())
                                    {
                                        metricInfo.id = metricCfg["id"].GetInt();
                                    }

                                    if (metricCfg.HasMember("name") && metricCfg["name"].IsString())
                                    {
                                        metricInfo.name = metricCfg["name"].GetString();
                                    }

                                    if (metricCfg.HasMember("aggregator") && metricCfg["aggregator"].IsString())
                                    {
                                        metricInfo.aggregator = metricCfg["aggregator"].GetString();
                                    }

                                    // 分位配置获取
                                    if (AGGREGATOR_QUANTILE == metricInfo.aggregator)
                                    {
                                        if (metricCfg.HasMember("k") && metricCfg["k"].IsInt())
                                        {
                                            metricInfo.k = metricCfg["k"].GetInt();
                                        }

                                        if (metricInfo.k <= 0)
                                        {
                                            metricInfo.k = DEFAULT_SKETCH_K;
                                        }

                                        if (metricCfg.HasMember("percentValueList") && metricCfg["percentValueList"].IsArray())
                                        {
                                            const rapidjson::Value &percentValueArray = metricCfg["percentValueList"];
                                            const size_t PERCENT_VALUE_ARRAY_SIZE(percentValueArray.Size());
                                            for (size_t k = 0; k < PERCENT_VALUE_ARRAY_SIZE; ++k)
                                            {
                                                const auto &percent = percentValueArray[k];
                                                if (percent.IsString())
                                                {
                                                    std::string p = percent.GetString();
                                                    if (!p.empty())
                                                    {
                                                        auto fraction = std::atof(p.c_str()) / FRACTION_UNIT;
                                                        metricInfo.vtFractions.emplace_back(fraction);
                                                        metricInfo.vtFractionName.emplace_back(p);
                                                    }
                                                }
                                            }
                                        }

                                        // 没有分位值，说明是上报摘要数据，因为格式兼容的问题，暂不支持
                                        if (metricInfo.vtFractions.empty())
                                        {
                                            continue; // 本次摘要指标直接忽略
                                        }
                                    }

                                    if (metricCfg.HasMember("outputName") && metricCfg["outputName"].IsString())
                                    {
                                        metricInfo.outputName = metricCfg["outputName"].GetString();
                                        ruleConfig.vtMetricInfo.emplace_back(std::move(metricInfo));
                                    }
                                }
                            }

                            mapRuleConfig.insert(std::pair<int, RuleConfig>(ruleConfig.ruleId, ruleConfig));
                        }
                    }

                    index = ctx.getRuleWriteIndex();
                    ctx.m_mapRuleId2Config[index] = std::move(mapRuleConfig);
                    ctx.swapRuleIndex();
                }

                LOG(INFO) << buf.str() << std::endl;

                return MONITOR_CPP_REPORT_SUCCESS;
            }
            else
            {
                LOG(ERROR) << "get monitor config list error! code: " << code << "|"
                           << "jsonStr: " << jsonStr << "|"
                           << std::endl;
            }

            LOG(INFO) << buf.str() << std::endl;
        }
        else
        {
            LOG(ERROR) << "parse failed json data: " << jsonStr << "|" << std::endl;
        }

        return MONITOR_CPP_REPORT_GET_CONFIG_FAILED;
    }

    int MonitorConfig::reportSdkVersion(MonitorContext &ctx)
    {
        std::string configUrl(ctx.m_monitorInitInfo.configUrl);
        const std::string oriStr("sdk-config");
        const std::string &url = (configUrl.replace(configUrl.find(oriStr), oriStr.length(), "saveSdkVersion"));
        if (url.empty())
        {
            LOG(ERROR) << "reportSdkVersion url empty!" << url << "|"
                       << std::endl;

            return MONITOR_CPP_REPORT_CONFIG_URL_EMPTY;
        }

        rapidjson::Document doc;
        doc.SetObject();
        rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
        doc.AddMember("version", rapidjson::Value().SetString(MONITOR_SDK_VERSION.c_str(), MONITOR_SDK_VERSION.length()), allocator);
        doc.AddMember("hostIp", rapidjson::Value().SetString(ctx.m_serverInfo.ip.c_str(), ctx.m_serverInfo.ip.length()), allocator);
        doc.AddMember("clientType", rapidjson::Value().SetString(MONITOR_LANGUAGE_TYPE.c_str(), MONITOR_LANGUAGE_TYPE.length()), allocator);
        doc.AddMember("appCode", rapidjson::Value().SetString(ctx.m_appCode.c_str(), ctx.m_appCode.length()), allocator);

        rapidjson::StringBuffer strBuf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
        doc.Accept(writer);
        common::HttpResponse response;
        common::HttpClient::Options options;
        options.AddHeader("Content-Type", "application/json;charset=UTF-8");
        //options.AddHeader("Accept", "application/json");
        options.AddHeader("spaceId", ctx.m_monitorInitInfo.spaceId);
        std::chrono::milliseconds ms = NOW_MILLI_SECONDS;
        std::ostringstream buf;
        buf << ms.count();
        options.AddHeader("timestamp", buf.str());
        std::string md5(md5str(ctx.m_monitorInitInfo.spaceId + buf.str() + ctx.m_monitorInitInfo.token));
        buf.str("");
        buf << ms.count() - 60 * 1000;
        options.AddHeader("lastScheduledTimeStamp", "28800000");
        options.AddHeader("md5", md5);
        common::HttpClient::ErrorCode error;
        std::string data = strBuf.GetString();
        ctx.m_client.Post(url, data, options, &response, &error);

        const int status{response.Status()};
        if (common::HttpClient::SUCCESS == error && common::HttpResponse::Status_OK == status)
        {
            LOG(INFO) << "reportSdkVersion url: " << url << "|"
                      << "token: " << ctx.m_monitorInitInfo.token << "|"
                      << "spaceId: " << ctx.m_monitorInitInfo.spaceId << "|"
                      << "timestamp: " << ms.count() << "|"
                      << "md5: " << md5 << "|"
                      << "request data: " << data << "|"
                      << "http status: " << status << "|"
                      << "error: " << error << "|"
                      << std::endl;
        }
        else
        {
            LOG(ERROR) << "reportSdkVersion url: " << url << "|"
                       << "token: " << ctx.m_monitorInitInfo.token << "|"
                       << "spaceId: " << ctx.m_monitorInitInfo.spaceId << "|"
                       << "timestamp: " << ms.count() << "|"
                       << "md5: " << md5 << "|"
                       << "request data: " << data << "|"
                       << "http status: " << status << "|"
                       << "error: " << error << "|"
                       << std::endl;
        }

        return MONITOR_CPP_REPORT_SUCCESS;
    }
}