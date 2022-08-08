//
// Created by 郑斌 on 2020-6-12.
//

#include "monitor_api.h"
#include <random>
#include <time.h>
#include <iostream>

class TestMonitorApi: public MonitorApi
{
public:
    TestMonitorApi(uint32_t maxThreadCnt, uint32_t reportInterval):
        //1.选择上报环境：REPORT_TEST_ENV(测试环境上报), 默认：REPORT_PRD_ENV(预发/正式环境上报)
        MonitorApi(maxThreadCnt, reportInterval, REPORT_TEST_ENV)
    {
    }

    virtual void LogReport(const std::string& str)
    {
        std::cout << str << std::endl;
    }

    virtual void LogError(const std::string& str)
    {
        std::cout << str << std::endl;
    }
};

void Report(TestMonitorApi& monitorApi)
{
    static thread_local uint32_t sum = 0;
    static thread_local time_t last = 0;
    sum++;
    time_t cur = time(NULL);
    if (cur > last)
    {
        monitorApi.Add(1, "sumReport1", sum);
        monitorApi.Add(1, "sumReport2", sum);
        monitorApi.Add(1, "sumReport3", sum);
        monitorApi.Add(1, "sumReport4", sum);
        monitorApi.Add(1, "sumReport5", sum);
        monitorApi.Add(1, "sumReport6", sum);
        monitorApi.Add(1, "sumReport7", sum);
        monitorApi.Add(1, "sumReport8", sum);
        monitorApi.Add(1, "sumReport9", sum);
        monitorApi.Add(1, "sumReport10", sum);
        sum = 0;
        last = cur;
    }
}

void ThreadReport(TestMonitorApi& monitorApi)
{
    std::default_random_engine dre(pthread_self());
    while(true)
    {
        // time_t curTime = time(NULL);
        // if (curTime % 10 > 2 && curTime % 10 < 8)
        // {
        //     for (size_t i = 0; i < 1000; i++)
        //     {
                //uint32_t val = dre() % 100;
                //uint32_t val = 1;
                // monitorApi.Add(1, "sum", 1);
                // monitorApi.AddAvg(1, "avg", val);
                // monitorApi.Set(2, "set", val);
                // monitorApi.SetMax(2, "max", val);
                // monitorApi.SetMin(2, "min", val);
        //     }
        // }
        // sleep(1);
        monitorApi.Add(1, "sumReport1", 1);
        monitorApi.Add(1, "sumReport2", 1);
        monitorApi.AddAvg(1, "avgReport3", 1);
        monitorApi.AddAvg(1, "avgReport4", 1);
        monitorApi.Set(1, "setReport5", 1);
        monitorApi.Set(1, "setReport6", 1);
        monitorApi.SetMax(1, "maxReport7", 1);
        monitorApi.SetMax(1, "maxReport8", 1);
        monitorApi.SetMin(1, "minReport9", 1);
        monitorApi.SetMin(1, "minReport10", 1);

        //Report(monitorApi);
    }
}

int main()
{
    TestMonitorApi monitorApi(100, 10);         // 线程数：100，上报间隔：10s
    std::map<std::string, std::string> dimMap;  // 设置维度
    dimMap["type"] = "test";                    // 维度为"type"，维度值为"test"
    // 设置通用维度，默认已有ip维度
    monitorApi.SetComDims(dimMap);
    // 设置上报ruleId
    std::vector<uint32_t> ruleIds;
    ruleIds.push_back(1);       //ruleId为1
    ruleIds.push_back(2);       //ruleId为2
    monitorApi.SetRuleIds(ruleIds);

    std::thread threads[32];
    for (uint32_t i = 0; i < 32; i++)
    {
        threads[i] = std::thread(ThreadReport, std::ref(monitorApi)); //线程采集上报
    }

    threads[0].join();
    return 0;
}
