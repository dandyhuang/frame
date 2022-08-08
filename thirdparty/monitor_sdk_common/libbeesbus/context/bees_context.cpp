//
// Created by 杨民善 on 2020/6/16.
//

#include <stdio.h>

#include "bees_context.h"

namespace vivo_bees_bus_cpp_report
{
    BeesContext::BeesContext() : m_isTerminate(true), m_loopInterval(DEFAULT_INTERVAL)
    {}

    bool BeesContext::getNextValidClient(const std::string &taskName, std::string &topic, std::string &tag, ThriftClientInfo &info)
    {
        auto makeClient = [&info] (BusInfo &busInfo) {
            std::vector<ThriftClientInfo> &vtValidClient = busInfo.vtThriftClientInfo;
            if (vtValidClient.empty())
            {
                return false;
            }

            bool isOpen{false};
            const size_t SIZE{vtValidClient.size()};
            info = vtValidClient[busInfo.clientIndex++ % SIZE];
            ThriftClient *pClient = info.pThriftClient;
            if (pClient)
            {
                LOG(INFO) << "valid host: " << pClient->getHost() << std::endl;
                pClient->getClient();
                if (!pClient->getTransPort()->isOpen())
                {
                    __VIVO_TRY__

                        pClient->getTransPort()->open();

                    __VIVO_CATCH_WITH_EXCEPTION__
                }

                isOpen = true;
            }

            return isOpen;
        };

        bool isSuccess{false};
        if (VIVO_BEES_CPP_REPORT_METHOD_BEES == m_method)
        {
            if (taskName.empty())
            {
                return isSuccess;
            }

            const auto &mapTask = getReadMapTask();
            const auto iter = mapTask.find(taskName);
            if (mapTask.end() != iter)
            {
                topic = iter->second.topic;
                auto &mapTag = getReadMapTag();
                auto tagIter = mapTag.find(iter->second.tag);
                if (mapTag.end() != tagIter)
                {
                    auto &busInfo = tagIter->second;
                    tag = tagIter->first;
                    isSuccess = makeClient(busInfo);
                }
            }
        }
        else
        {
            BusInfo &busInfo = getReadFlume();
            isSuccess = makeClient(busInfo);
        }

        return isSuccess;
    }

    TaskMap& BeesContext::getWriteMapTask()
    {
        return m_mapTask2Tag[getMapWriteIndex()];
    }

    TaskMap& BeesContext::getReadMapTask()
    {
        return m_mapTask2Tag[getMapReadIndex()];
    }

    size_t BeesContext::getMapWriteIndex()
    {
        return (m_currentMapIndex + 1) % MAX_SIZE;
    }

    size_t BeesContext::getMapReadIndex()
    {
        return m_currentMapIndex;
    }

    size_t BeesContext::getFlumeWriteIndex()
    {
        return (m_currentFlumeIndex + 1) % MAX_SIZE;
    }

    size_t BeesContext::getFlumeReadIndex()
    {
        return m_currentFlumeIndex;
    }

    void BeesContext::swapMap()
    {
        m_currentMapIndex = (m_currentMapIndex + 1) % MAX_SIZE;
    }

    TagMap & BeesContext::getWriteMapTag()
    {
        return m_mapTag2BusInfo[getMapWriteIndex()];
    }

    TagMap & BeesContext::getReadMapTag()
    {
        return m_mapTag2BusInfo[getMapReadIndex()];
    }

    void BeesContext::swapFlume()
    {
        m_currentFlumeIndex = (m_currentFlumeIndex + 1) % MAX_SIZE;
    }

    BusInfo& BeesContext::getWriteFlume()
    {
        return m_vtValidClient[getFlumeWriteIndex()];
    }

    BusInfo& BeesContext::getReadFlume()
    {
        return m_vtValidClient[getFlumeReadIndex()];
    }

    bool BeesContext::addTask(const std::string &taskName)
    {
        if (taskName.empty())
        {
            return false;
        }

        std::lock_guard<std::mutex> lockGuard(m_setBeesTaskNameMutex);
        auto result = m_setBeesTaskName.insert(taskName);

        if (result.second)
        {
            m_lastUpdateTime = 0;
        }

        return result.second;
    }

    bool BeesContext::delTask(const std::string &taskName)
    {
        if (taskName.empty())
        {
            return false;
        }

        std::lock_guard<std::mutex> lockGuard(m_setBeesTaskNameMutex);
        auto size = m_setBeesTaskName.erase(taskName);
        bool result{size > 0};
        if (result)
        {
            m_lastUpdateTime = 0;
        }

        return result;
    }

    std::string BeesContext::getLocalIp()
    {
        // 获取一次之后，直接返回
        if(!m_report2BeesInfo.localIp.empty())
        {
            return m_report2BeesInfo.localIp;
        }

        m_report2BeesInfo.localIp = vivo_bees_bus_cpp_report::getLocalIp();

        return m_report2BeesInfo.localIp;
    }

    void BeesContext::releaseClient(std::vector<ThriftClientInfo> &vtThriftClient)
    {
        if (vtThriftClient.empty())
        {
            return;
        }

        const size_t SIZE{vtThriftClient.size()};
        for (size_t i = 0; i < SIZE; ++i)
        {
            if (vtThriftClient[i].pThriftClient)
            {
                delete vtThriftClient[i].pThriftClient;
                vtThriftClient[i].pThriftClient = nullptr;
            }

            if (vtThriftClient[i].pThriftMutex)
            {
                delete vtThriftClient[i].pThriftMutex;
                vtThriftClient[i].pThriftMutex = nullptr;
            }
        }

        vtThriftClient.clear();
    }
}