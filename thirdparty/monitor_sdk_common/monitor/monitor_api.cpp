//
// Created by 郑斌 on 2020-6-12.
//

#include "monitor_api.h"
#include <time.h>
#include <unistd.h>
#include <sstream>
#include "thirdparty/monitor_sdk_common/system/net/ip_address.h"

MonitorApi::StatInfo::StatInfo()
{
    type = STAT_TYPE_SUM;
    val = 0;
    cnt = 0;
}

void MonitorApi::StatInfo::Add(const StatInfo& info)
{
    Add(info.type, info.val, info.cnt);
}

void MonitorApi::StatInfo::Add(StatInfo::StatType t, int64_t v, uint32_t c)
{
    StatType oldType = type;
    type = t;
    cnt += c;   
    switch(type)
    {
        case STAT_TYPE_SET:
            val = v;
            break;
        case STAT_TYPE_MAX:
            val = (v > val ? v : val);
            break;
        case STAT_TYPE_MIN:
            if (oldType != STAT_TYPE_MIN)
            {
                val = v;
            }
            else
            {
                val = (v < val ? v : val);
            }
            break;
        default:
            val += v;
            break;
    }
}

int64_t MonitorApi::StatInfo::GetResult()
{
    switch(type)
    {
        case STAT_TYPE_AVG:
            return (cnt > 0 ? val / cnt : 0);
        default:
            return val;
    }
}

uint32_t MonitorApi::m_threadCnt = 0;
std::mutex MonitorApi::m_mutex;

MonitorApi::MonitorApi(uint32_t maxThreadCnt, uint32_t reportInterval, const std::string& reportUrl):
    m_isStop(false),
    m_reportUrl(reportUrl),
    m_maxThreadCnt(maxThreadCnt),
    m_reportInterval(reportInterval),
    m_batchNum(100),
    m_cacheNum(1000),
    m_isStatEnable(true),
    m_isReportEnable(true),
    m_ruleStatMapIdx(0)
{
    if (m_maxThreadCnt == 0)
    {
        m_maxThreadCnt = 1;
    }
    if (m_maxThreadCnt == 1)
    {
        m_threadCnt = 1;
    }
    if (m_reportInterval == 0)
    {
        m_reportInterval = 1;
    }
    common::IpAddress addr;
    common::IpAddress::GetFirstPrivateAddress(&addr);
    m_ip = addr.ToString();

    m_reportThread = std::thread(&MonitorApi::ReportLoop, this);
}

MonitorApi::~MonitorApi()
{
    m_isStop = true;
    m_reportThread.join();

    for (auto& ruleStatMap: m_ruleStatMaps)
    {
        for (auto it: ruleStatMap)
        {
            delete[] it.second;
        }
    }
}

void MonitorApi::SetRuleIds(const std::vector<uint32_t>& ruleIds)
{
    for (uint32_t ruleId: ruleIds)
    {
        AddRuleId(ruleId);
    }
}

void MonitorApi::AddRuleId(uint32_t ruleId)
{
    for (auto& ruleStatMap : m_ruleStatMaps)
    {
        if (ruleStatMap.find(ruleId) != ruleStatMap.end())
        {
            continue;
        }

        ruleStatMap[ruleId] = new StatMap[m_maxThreadCnt];
    }
}

void MonitorApi::SetComDims(const std::map<std::string, std::string>& dimMap)
{
    m_comDimMap = dimMap;
}

void MonitorApi::ReportLoop()
{
    time_t lastReportTime = time(NULL) / 60 * 60;
    bool isQuit = false;
    while (true)
    {
        time_t curTime = time(NULL);
        if (m_isStop)
        {
            isQuit = true;
        }
        if (!isQuit && (curTime < lastReportTime + m_reportInterval))
        {
            if (Report() == 0)
            {
                usleep(100000);
            }

            continue;
        }

        Stat();
        Report();
        lastReportTime = curTime;

        if (isQuit)
        {
            break;
        }
    }
}

void MonitorApi::Stat()
{
    int64_t timeStamp = ((time(NULL) - 1) / 60 * 60) * 1000;
    SwitchRuleStatMap();
    //sleep保证老的map不会再写入
    sleep(1);

    RuleStatMap& ruleStatMap = GetNextRuleStatMap();
    std::map<uint32_t, StatMap> ruleStat;
    for (auto it: ruleStatMap)
    {
        ruleStat[it.first]["report"].Add(StatInfo::STAT_TYPE_SUM, 1);
        for (uint32_t i = 0; i < m_threadCnt && i < m_maxThreadCnt; i++)
        {
            for (auto itStat: it.second[i])
            {
                ruleStat[it.first][itStat.first].Add(itStat.second);
            }
            it.second[i].clear();
        }
    }

    for (auto it: ruleStat)
    {
        std::ostringstream oss;
        oss << "{\"rule_id\":" << it.first
            << ",\"timestamp\":\"" << timeStamp << "\""
            << ",\"ip\":\"" << m_ip << "\"";
        for (auto itDim: m_comDimMap)
        {
            oss << ",\"" << itDim.first << "\":\"" << itDim.second << "\"";
        }
        for (auto itStat: it.second)
        {
            oss << ",\"" << itStat.first << "\":\"" << itStat.second.GetResult() << "\"";
        }
        oss << "}";
        if (m_reportDatas.size() >= m_cacheNum)
        {
            m_reportDatas.pop_front();
        }
        m_reportDatas.push_back(oss.str());
        LogReport(oss.str());
    }
}

uint32_t MonitorApi::Report()
{
    uint32_t reportCnt = 0;
    do
    {
        if (m_reportDatas.empty())
        {
            break;
        }

        std::string data;
        data += "[";
        uint32_t cnt = 0;
        for (auto it = m_reportDatas.begin(); it != m_reportDatas.end() && cnt < m_batchNum; it++)
        {
            if (data.size() > 1)
            {
                data += ",";
            }
            data += *it;
            cnt++;
        }
        data += "]";

        if (m_isReportEnable)
        {
            common::HttpClient::Options options;
            options.AddHeader("Content-Type", "application/json");
            common::HttpResponse httpRsp;
            common::HttpClient::ErrorCode errCode;
            std::ostringstream oss;
            LogDebug(data);
            if (!m_httpCliet.Post(m_reportUrl, data, options, &httpRsp, &errCode))
            {
                oss << "http post error: " << m_httpCliet.GetErrorMessage(errCode) 
                    << ", report num: " << cnt
                    << ", cache num: " << m_reportDatas.size();
                LogError(oss.str());
                break;
            }
            if (httpRsp.Status() != common::HttpResponse::Status_OK)
            {
                oss << "invalid http status code: " << httpRsp.Status()
                    << ", report num: " << cnt
                    << ", cache num: " << m_reportDatas.size();
                LogError(oss.str());
                break;
            }
        }

        for (uint32_t i = 0; i < cnt; i++)
        {
            m_reportDatas.pop_front();
        }

        reportCnt += cnt;
    } while (m_isStop);
    
    return reportCnt;
}

uint32_t MonitorApi::GetThreadIndex()
{
    if (m_maxThreadCnt <= 1)
    {
        return 0;
    }

    static thread_local int32_t threadIndex = -1;
    if (threadIndex >= 0)
    {
        if ((uint32_t)threadIndex < m_maxThreadCnt)
        {
            return threadIndex;
        }
        else
        {
            return 0;
        }
        
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_threadCnt >= m_maxThreadCnt)
    {
        threadIndex = 0;
        return threadIndex;
    }
    threadIndex = m_threadCnt++;
    return threadIndex;
}

MonitorApi::RuleStatMap& MonitorApi::GetRuleStatMap()
{
    return m_ruleStatMaps[m_ruleStatMapIdx];
}

MonitorApi::RuleStatMap& MonitorApi::GetNextRuleStatMap()
{
    return m_ruleStatMaps[1 - m_ruleStatMapIdx];
}

void MonitorApi::SwitchRuleStatMap()
{
    m_ruleStatMapIdx = 1 - m_ruleStatMapIdx;
}

void MonitorApi::Add(uint32_t ruleId, const std::string& key, StatInfo::StatType type, int64_t val)
{
    if (!m_isStatEnable)
    {
        return;
    }

    RuleStatMap& ruleStatMap = GetRuleStatMap();
    uint32_t index = GetThreadIndex();
    auto itStatMap = ruleStatMap.find(ruleId);
    if (itStatMap == ruleStatMap.end())
    {
        return;
    }

    StatMap& statMap = itStatMap->second[index];
    statMap[key].Add(type, val);
}

void MonitorApi::Add(uint32_t ruleId, const std::string& key, int64_t val)
{
    Add(ruleId, key, StatInfo::STAT_TYPE_SUM, val);
}

void  MonitorApi::AddAvg(uint32_t ruleId, const std::string& key, int64_t val)
{
    Add(ruleId, key, StatInfo::STAT_TYPE_AVG, val);
}

void MonitorApi::Set(uint32_t ruleId, const std::string& key, int64_t val)
{
    Add(ruleId, key, StatInfo::STAT_TYPE_SET, val);
}

void MonitorApi::SetMax(uint32_t ruleId, const std::string& key, int64_t val)
{
    Add(ruleId, key, StatInfo::STAT_TYPE_MAX, val);
}

void MonitorApi::SetMin(uint32_t ruleId, const std::string& key, int64_t val)
{
    Add(ruleId, key, StatInfo::STAT_TYPE_MIN, val);
}
