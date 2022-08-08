//
// Created by 郑斌 on 2020-6-12.
//

#pragma once

#include <string>
#include <vector>
#include <map>
#include <list>
#include <thread>
#include <mutex>
#include "thirdparty/monitor_sdk_common/net/http/client.h"

#define REPORT_TEST_ENV "http://10.101.51.8:8082"                       //上报测试环境
#define REPORT_PRD_ENV "http://st-moni-business-shou.vivo.lan:8080"     //上报预发/正式环境

class MonitorApi
{
public:
    //maxThreadCnt: 最大线程数，如果小于实际线程数，可能会导致结果不准确
    //reportInterval: 上报间隔，单位秒
    //reportUrl: 上报地址，默认上报正式环境
    MonitorApi(uint32_t maxThreadCnt = 1, uint32_t reportInterval = 60,
        const std::string& reportUrl = REPORT_PRD_ENV);
    virtual ~MonitorApi();

    //批量上报数据条数，默认100
    void SetBatchNum(uint32_t batchNum) { m_batchNum = batchNum; }
    //设置上报失败时缓存的数据条数，默认10000
    void SetCacheNum(uint32_t cacheNum) { m_cacheNum = cacheNum; }
    //设置ruleId，所有上报的ruleId都要在初始化时设置，否则上报无效，多次设置取并集；所有ruleId每个周期会上报一次key为report的指标
    void SetRuleIds(const std::vector<uint32_t>& ruleIds);
    //添加ruleId
    void AddRuleId(uint32_t ruleId);
    //设置通用维度，初始化时设置，默认会上报ip
    void SetComDims(const std::map<std::string, std::string>& dimMap);
    //启用/禁用统计
    void EnableStat(bool isEnable) { m_isStatEnable = isEnable; }
    //启用/禁用上报
    void EnableReport(bool isEnable) { m_isReportEnable = isEnable; }
    //获取上报状态
    bool IsReportEnable() { return m_isReportEnable; }

    //上报累加值
    void Add(uint32_t ruleId, const std::string& key, int64_t val);
    //上报平均值
    void AddAvg(uint32_t ruleId, const std::string& key, int64_t val);
    //上报最新值
    void Set(uint32_t ruleId, const std::string& key, int64_t val);
    //上报最大值
    void SetMax(uint32_t ruleId, const std::string& key, int64_t val);
    //上报最小值
    void SetMin(uint32_t ruleId, const std::string& key, int64_t val);

    //如果需要把上报的内容打到日志，实现该方法
    virtual void LogReport(const std::string& reportStr) {}
    //打印调试信息到日志
    virtual void LogDebug(const std::string& str) {}
    //打印错误信息到日志
    virtual void LogError(const std::string& str) {}

protected:
    struct StatInfo
    {
        enum StatType
        {
            STAT_TYPE_SUM = 0,
            STAT_TYPE_AVG,
            STAT_TYPE_SET,
            STAT_TYPE_MAX,
            STAT_TYPE_MIN, 
        };

        StatType type;
        int64_t val;
        uint32_t cnt;

        StatInfo();
        void Add(const StatInfo& info);
        void Add(StatType t, int64_t v, uint32_t c = 1);
        int64_t GetResult();
    };

    typedef std::map<std::string, std::string> DimMap;
    typedef std::map<std::string, StatInfo> StatMap;
    typedef std::map<uint32_t, StatMap*> RuleStatMap; 

    void ReportLoop();
    void Stat();
    uint32_t Report();

    uint32_t GetThreadIndex();

    RuleStatMap& GetRuleStatMap();
    RuleStatMap& GetNextRuleStatMap();
    void SwitchRuleStatMap();

    void Add(uint32_t ruleId, const std::string& key, StatInfo::StatType type, int64_t val);

protected:
    bool m_isStop;
    std::string m_reportUrl;
    uint32_t m_maxThreadCnt;
    uint32_t m_reportInterval;
    uint32_t m_batchNum;
    uint32_t m_cacheNum;
    std::string m_ip;
    std::thread m_reportThread;
    common::HttpClient m_httpCliet;
    bool m_isStatEnable;
    bool m_isReportEnable;

    DimMap m_comDimMap;
    RuleStatMap m_ruleStatMaps[2];
    uint32_t m_ruleStatMapIdx;

    static uint32_t m_threadCnt;
    static std::mutex m_mutex;

    std::list<std::string> m_reportDatas;
};
