//
// Created by 杨民善 on 2020/6/15.
//

#include "../util/util.h"
#include "report_to_bees.h"
#include "../context/bees_context.h"

#include <unistd.h>
#include <sstream>
#include <algorithm>
#include <random>

#include "thirdparty/rapidjson/document.h"
#include "thirdparty/rapidjson/stringbuffer.h"
#include "thirdparty/rapidjson/writer.h"

namespace vivo_bees_bus_cpp_report
{
    void Report2Bees::beesBusTask(BeesContext &ctx)
    {
        unsigned int loopInterval(ctx.m_loopInterval);
        while (!ctx.m_isTerminate)
        {
            auto now = BEES_NOW.count();
            if (now - ctx.m_lastUpdateTime > loopInterval || isNeedUpdate(ctx))
            {
                ctx.m_lastUpdateTime = now;
                int ret{refreshClusterList(ctx)};

                // 如果请求不成功则将轮询间隔调小
                loopInterval = (VIVO_BEES_CPP_REPORT_SUCCESS == ret ? ctx.m_loopInterval.load() : DEFAULT_QUICK_INTERVAL);
            }
            else
            {
                checkAndRebuildConnection(ctx);
            }

            std::this_thread::sleep_for(std::chrono::seconds(DEFAULT_SECOND));
        }
    }

    int Report2Bees::refreshClusterList(BeesContext &ctx)
    {
        int ret(VIVO_BEES_CPP_REPORT_METHOD_BEES == ctx.m_method ? getBusListInfo(ctx) : getFlumeListInfo(ctx));

        return ret;
    }

    BeesBusReportStatus Report2Bees::append(BeesContext &ctx, const AppendContext &appendContext, BeesBusThriftFlumeEvent &event)
    {
        if (!event.body.empty())
        {
            std::string topic;
            std::string tag;
            ThriftClientInfo info;
            if (ctx.getNextValidClient(appendContext.taskName, topic, tag,info))
            {
                __VIVO_TRY__

                    fillHeader(ctx, appendContext.taskName, topic, tag, event);

                    auto ret(com::vivo::bees::bus::thrift::Status::UNKNOWN);
                    if (info.pThriftClient && info.pThriftMutex)
                    {
                        std::lock_guard<std::mutex> lockGuard(*info.pThriftMutex);
                        ret = info.pThriftClient->getClient()->append(event);
                        if (com::vivo::bees::bus::thrift::Status::OK != ret)
                        {
                            ++ctx.m_failCount;
                        }
                    }

                    return ret;

                __VIVO_CATCH_WITH_EXCEPTION__
            }
            else
            {
                LOG(ERROR) << "pThriftClient empty!" << std::endl;
            }
        }
        else
        {
            LOG(ERROR) << "body empty! headers size: " << event.headers.size() << "|"
                       << "body size: " << event.body.size() << "|"
                       << "taskName: " << appendContext.taskName << "|"
                       << std::endl;
        }

        return com::vivo::bees::bus::thrift::Status::UNKNOWN;
    }

    BeesBusReportStatus Report2Bees::appendBatch(BeesContext &ctx, const AppendContext &appendContext, std::vector<BeesBusThriftFlumeEvent> &events)
    {
        if (!events.empty())
        {
            std::string topic;
            std::string tag;
            ThriftClientInfo info;
            if (ctx.getNextValidClient(appendContext.taskName, topic, tag, info))
            {
                __VIVO_TRY__
                    if (!topic.empty())
                    {
                        for (auto &event : events)
                        {
                            fillHeader(ctx, appendContext.taskName, topic, tag, event);
                        }
                    }

                    auto ret(com::vivo::bees::bus::thrift::Status::UNKNOWN);
                    if (info.pThriftClient && info.pThriftMutex)
                    {
                        std::lock_guard<std::mutex> lockGuard(*info.pThriftMutex);
                        ret = info.pThriftClient->getClient()->appendBatch(events);
                        if (com::vivo::bees::bus::thrift::Status::OK != ret)
                        {
                            ++ctx.m_failCount;
                        }
                    }

                    return ret;

                __VIVO_CATCH_WITH_EXCEPTION__
            }
            else
            {
                LOG(ERROR) << "pThriftClient empty!" << std::endl;
            }
        }
        else
        {
            LOG(ERROR) << "events empty! events size: " << events.size() << "|"
                       << "taskName: " << appendContext.taskName << "|"
                       << std::endl;
        }

        return com::vivo::bees::bus::thrift::Status::UNKNOWN;
    }

    bool Report2Bees::isNeedUpdate(BeesContext &ctx)
    {
        bool isNeed{false};

        if (VIVO_BEES_CPP_REPORT_METHOD_FLUME == ctx.m_method)
        {
            return isNeed;
        }

        auto &mapWrite = ctx.getWriteMapTask();
        if (mapWrite.empty())
        {
            isNeed = true;
        }
        else
        {
            auto &mapRead = ctx.getReadMapTask();
            if (mapWrite.size() != mapRead.size())
            {
                isNeed = true;
            }
            else
            {
                auto isSame = [](TaskMap &l, TaskMap &r) {
                    auto lIter = l.begin();
                    while (l.end() != lIter)
                    {
                        auto rIter = r.find(lIter->first);
                        if (r.end() == rIter)
                        {
                            return false;
                        }

                        if (lIter->second.version != rIter->second.version)
                        {
                            return false;
                        }

                        ++lIter;
                    }

                    return true;
                };

                isNeed = !isSame(mapWrite, mapRead);
                if (!isNeed)
                {
                    isNeed = !isSame(mapRead, mapWrite);
                }
            }
        }

        return isNeed;
    }

    bool Report2Bees::reviseTask(BeesContext &ctx)
    {
        bool isModify{false};
        auto &mapTask = ctx.getWriteMapTask();
        std::lock_guard<std::mutex> lockGuard(ctx.m_setBeesTaskNameMutex);

        auto taskNameIter = ctx.m_setBeesTaskName.begin();
        while (ctx.m_setBeesTaskName.end() != taskNameIter)
        {
            auto iter = mapTask.find(*taskNameIter);
            if (mapTask.end() == iter)
            {
                isModify = true;
                mapTask[*taskNameIter] = TaskInfo{};
            }

            ++taskNameIter;
        }

        auto taskIter = mapTask.begin();
        while (mapTask.end() != taskIter)
        {
            auto iter = ctx.m_setBeesTaskName.find(taskIter->first);
            if (ctx.m_setBeesTaskName.end() == iter)
            {
                isModify = true;
                mapTask.erase(taskIter++);
            }
            else
            {
                ++taskIter;
            }
        }

        return isModify;
    }

    bool Report2Bees::reviseTag(BeesContext &ctx, const std::set<std::string> &setTag)
    {
        bool isModify{false};
        auto &mapTag = ctx.getWriteMapTag();
        auto tagIter = mapTag.begin();
        while (mapTag.end() != tagIter)
        {
            auto iter = setTag.find(tagIter->first);
            if (setTag.end() == iter)
            {
                isModify = true;
                BeesContext::releaseClient(tagIter->second.vtThriftClientInfo);
                mapTag.erase(tagIter++);
            }
            else
            {
                ++tagIter;
            }
        }

        return isModify;
    }

    void Report2Bees::checkAndRebuildConnection(BeesContext &ctx)
    {
        if (ctx.m_failCount > DEFAULT_FAIL_COUNT)
        {
            ctx.m_failCount = 0; // 在重建之前清 0，避免重复重建
            rebuildConnection(ctx);
        }
    }

    void Report2Bees::rebuildConnection(BeesContext &ctx)
    {
        if (VIVO_BEES_CPP_REPORT_METHOD_BEES == ctx.m_method)
        {
            auto &mapTag = ctx.getWriteMapTag();
            auto iter = mapTag.begin();
            while (mapTag.end() != iter)
            {
                BeesContext::releaseClient(iter->second.vtThriftClientInfo);
                buildConnection(ctx, iter->second.vtThriftClientInfo, iter->second.vtHost, BEES_THRIFT_PORT);
                ++iter;
            }

            ctx.swapMap();
        }
        else
        {
            auto &busInfo = ctx.getWriteFlume();
            BeesContext::releaseClient(busInfo.vtThriftClientInfo);
            buildConnection(ctx, busInfo.vtThriftClientInfo, ctx.m_vtFlumeIp, ctx.m_flumePort);

            ctx.swapFlume();
        }
    }

    int Report2Bees::getBusListInfo(BeesContext &ctx)
    {
        const std::string &beesManagerUrl = ctx.m_report2BeesInfo.beesManagerUrl;
        if (beesManagerUrl.empty())
        {
            LOG(ERROR) << "get cluster list failed, beesManagerUrl empty: " << beesManagerUrl << "|"
                       << std::endl;
            return VIVO_BEES_CPP_REPORT_PARAM_EMPTY;
        }

        rapidjson::Document doc;
        doc.SetObject();
        rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
        rapidjson::Value strValue(rapidjson::kStringType);
        strValue.SetString(ctx.m_report2BeesInfo.localIp.c_str(), ctx.m_report2BeesInfo.localIp.size());
        if (!strValue.IsNull())
        {
            doc.AddMember("hostIp", strValue, allocator);
        }

        doc.AddMember("networkType", 0, allocator);

        rapidjson::Value taskList(rapidjson::kArrayType);
        std::set<std::string> setTag;
        bool isTaskModify{reviseTask(ctx)};
        auto &mapTask = ctx.getWriteMapTask();
        auto iter = mapTask.begin();
        while (mapTask.end() != iter)
        {
            rapidjson::Value taskInfo(rapidjson::kObjectType);
            strValue.SetString(iter->first.c_str(), iter->first.size());
            taskInfo.AddMember("taskName", strValue, allocator);
            taskInfo.AddMember("version", iter->second.version, allocator);
            taskList.PushBack(taskInfo, allocator);

            setTag.insert(iter->second.tag);

            ++iter;
        }

        bool isTagModify{false};
        if (isTaskModify)
        {
            isTagModify = reviseTag(ctx, setTag);
        }

        doc.AddMember("tasklist", taskList, allocator);

        rapidjson::StringBuffer strBuf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
        doc.Accept(writer);
        common::HttpResponse response;
        common::HttpClient::Options options;
        options.AddHeader("Content-Type", "application/json");
        options.AddHeader("Accept", "application/json");
        common::HttpClient::ErrorCode error;
        std::string data = strBuf.GetString();
        if (!setTag.empty())
        {
            ctx.m_client.Post(beesManagerUrl + GET_BUS_LIST, data, options, &response, &error);
        }

        const int status{response.Status()};
        if (common::HttpClient::SUCCESS == error && common::HttpResponse::Status_OK == status)
        {
            bool isUpdate{handleBusInfoResponse(ctx, response.Body())};
            if (!isUpdate)
            {
                rebuildConnection(ctx);
            }
            else
            {
                ctx.swapMap();
            }

            LOG(INFO) << "get bus list success beesManagerUrl: " << beesManagerUrl + GET_BUS_LIST << "|"
                      << "request data: " << data << "|"
                      << "isTaskModify: " << isTaskModify << "|"
                      << "isTagModify: " << isTagModify << "|"
                      << "isUpdate: " << isUpdate << "|"
                      << std::endl;

            return VIVO_BEES_CPP_REPORT_SUCCESS;
        }
        else
        {
            LOG(ERROR) << "get bus list failed beesManagerUrl: " << beesManagerUrl + GET_BUS_LIST << "|"
                       << "request data: " << data << "|"
                       << "isTaskModify: " << isTaskModify << "|"
                       << "isTagModify: " << isTagModify << "|"
                       << "http status: " << status << "|"
                       << "error: " << error << std::endl;
        }

        rebuildConnection(ctx);

        return VIVO_BEES_CPP_REPORT_GET_CLUSTER_FAILED;
    }

    int Report2Bees::getFlumeListInfo(BeesContext &ctx)
    {
        BusInfo &busInfo = ctx.getWriteFlume();
        BeesContext::releaseClient(busInfo.vtThriftClientInfo);
        int ret{buildConnection(ctx, busInfo.vtThriftClientInfo, ctx.m_vtFlumeIp, ctx.m_flumePort)};

        ctx.swapFlume();

        return ret;
    }

    bool Report2Bees::handleBusInfoResponse(BeesContext &ctx, const std::string &jsonStr)
    {
        bool isModify{false};
        rapidjson::Document doc;
        std::ostringstream buf;
        buf << "jsonStr: " << jsonStr << "|";
        if (!doc.Parse(jsonStr.data()).HasParseError())
        {
            int code(DEFAULT_ERROR_CODE);
            if(doc.HasMember("code") && doc["code"].IsInt())
            {
                code = doc["code"].GetInt();
            }

            buf << "code: " << code << "|";
            if (0 == code)
            {
                if (doc.HasMember("data") && doc["data"].IsArray())
                {
                    const rapidjson::Value &data = doc["data"];
                    const size_t DATA_SIZE(data.Size());
                    buf << "data size: " << DATA_SIZE << "|";
                    for (size_t i = 0; i < DATA_SIZE; ++i)
                    {
                        const auto &dataItem = data[i];
                        if (dataItem.IsObject())
                        {
                            std::string taskName;
                            TaskInfo taskInfo;
                            if (dataItem.HasMember("taskName") && dataItem["taskName"].IsString())
                            {
                                taskName = dataItem["taskName"].GetString();
                            }

                            if (dataItem.HasMember("topic") && dataItem["topic"].IsString())
                            {
                                taskInfo.topic = dataItem["topic"].GetString();
                            }

                            if (dataItem.HasMember("busTagInfo"))
                            {
                                const auto &busTagInfo = dataItem["busTagInfo"];
                                if (busTagInfo.IsObject())
                                {
                                    if (busTagInfo.HasMember("version") && busTagInfo["version"].IsInt64())
                                    {
                                        taskInfo.version = busTagInfo["version"].GetInt64();
                                    }

                                    if (busTagInfo.HasMember("busTagName") && busTagInfo["busTagName"].IsString())
                                    {
                                        taskInfo.tag = busTagInfo["busTagName"].GetString();
                                        auto &mapTask = ctx.getWriteMapTask();
                                        auto iter = mapTask.find(taskName);
                                        if (mapTask.end() != iter)
                                        {
                                            iter->second = taskInfo;
                                        }
                                        else
                                        {
                                            mapTask.emplace(std::make_pair(taskName, taskInfo));
                                        }
                                    }

                                    if (busTagInfo.HasMember("hosts")  && busTagInfo["hosts"].IsArray())
                                    {
                                        const auto &hosts = busTagInfo["hosts"];
                                        const size_t HOST_SIZE(hosts.Size());
                                        std::vector<std::string> vtHost;
                                        for (size_t j = 0; j < HOST_SIZE && hosts[j].IsString(); ++j)
                                        {
                                            const std::string &host{hosts[j].GetString()};
                                            if (!host.empty())
                                            {
                                                vtHost.emplace_back(host);
                                            }

                                            buf << host << "; ";
                                        }

                                        if (!vtHost.empty())
                                        {
                                            auto &mapTag = ctx.getWriteMapTag();
                                            auto tagIter = mapTag.find(taskInfo.tag);
                                            isModify = true;
                                            if (mapTag.end() != tagIter)
                                            {
                                                tagIter->second.clientIndex = 0;
                                                tagIter->second.vtHost = std::move(vtHost);
                                                BeesContext::releaseClient(tagIter->second.vtThriftClientInfo);
                                                buildConnection(ctx, tagIter->second.vtThriftClientInfo, tagIter->second.vtHost, BEES_THRIFT_PORT);
                                            }
                                            else
                                            {
                                                BusInfo info;
                                                info.vtHost = std::move(vtHost);
                                                buildConnection(ctx, info.vtThriftClientInfo, info.vtHost, BEES_THRIFT_PORT);
                                                mapTag.emplace(std::make_pair(taskInfo.tag, std::move(info)));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                LOG(INFO) << buf.str() << std::endl;
            }
            else
            {
                LOG(ERROR) << "get cluster list error! code: " << code << "|" << buf.str() << std::endl;
            }
        }
        else
        {
            LOG(ERROR) << "parse failed json data: " << jsonStr << "|" << std::endl;
        }

        return isModify;
    }

    int Report2Bees::buildConnection(BeesContext &ctx, std::vector<ThriftClientInfo> &vtThriftClient, std::vector<std::string> &vtHost, int port)
    {
        std::ostringstream buf;
        auto &vtClusterList = vtHost;
        const size_t SIZE{vtClusterList.size()};
        buf << "vtHost size: " << SIZE << "|";

        if (!vtClusterList.empty())
        {
            // 超过才需要随机
            if (SIZE >= MAX_COUNT)
            {
                std::random_device rd;
                std::mt19937 g(rd());
                std::shuffle(vtClusterList.begin(), vtClusterList.end(), g);
            }

            for (size_t i = 0; i < SIZE && i < MAX_COUNT; ++i)
            {
                auto *pClient = new ThriftClient(vtClusterList[i], port, ctx.m_report2BeesInfo.sendTimeout, ctx.m_report2BeesInfo.recvTimeout);
                auto *pMutex = new std::mutex();
                if (pClient)
                {
                    buf << vtClusterList[i] << ":" << ctx.m_report2BeesInfo.beesBusPort << "; ";

                    vtThriftClient.push_back(ThriftClientInfo{pClient, pMutex});
                }
            }

            if (!vtThriftClient.empty())
            {
                LOG(INFO) << buf.str() << std::endl;

                return VIVO_BEES_CPP_REPORT_SUCCESS;
            }
            else
            {
                LOG(ERROR) << buf.str() << "|vtThriftClient empty! may be new memory failed!" << std::endl;

                return VIVO_BEES_CPP_REPORT_NEW_THRIFT_CLIENT_FAILED;
            }
        }
        else
        {
            LOG(ERROR) << "vtHost empty!" << std::endl;
        }

        LOG(INFO) << buf.str() << std::endl;

        return VIVO_BEES_CPP_REPORT_CLUSTER_EMPTY;
    }

    void Report2Bees::fillHeader(BeesContext &ctx, const std::string &task, const std::string &topic, const std::string &tag, BeesBusThriftFlumeEvent &event)
    {
        if (!topic.empty())
        {
            event.headers[TOPIC_KEY] = topic;
        }

        if (!tag.empty())
        {
            event.headers[TAG_KEY] = tag;
        }

        if (event.headers[APP_NAME_KEY].empty() && !task.empty())
        {
            event.headers[APP_NAME_KEY] = task;
        }

        std::chrono::milliseconds ms = BEES_NOW_MILLI_SECONDS;
        std::ostringstream buf;
        buf << ms.count();
        event.headers[TIMESTAMP_KEY] = buf.str();
        event.headers[DATA_TIMESTAMP_KEY] = buf.str();
        event.headers[LINES_KEY] = "1";
        event.headers[IP_KEY] = getLocalIp();
        event.headers[HOST_NAME_KEY] = getHostName();
        event.headers[BYTES_KEY] = std::to_string(event.body.size());

        auto printHeader = [&buf] (std::map<std::string, std::string> &header) {
            buf.str("");
            buf << "header size: " << header.size() << "|";
            std::for_each(header.begin(), header.end(), [&buf](const std::pair<std::string, std::string> &headerPair){
                buf << headerPair.first << ":" << headerPair.second << ";";
            });

            return buf.str();
        };

        LOG(INFO) << printHeader(event.headers) << std::endl;
    }
}