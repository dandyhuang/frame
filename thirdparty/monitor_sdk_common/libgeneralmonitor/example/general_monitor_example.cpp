//
// Created by 杨民善 on 2020/6/22.
//

#include "general_monitor_example.h"

#include <thread>
#include <algorithm>
#include <random>
#include <iostream>

using namespace vivo_general_monitor_cpp_report;

//static const std::string TOKEN{"929853d8d7316efce087b4e4c1eb3c12"};                           // 通用监控 tars空间token
//static const std::string SPACE_ID{"60"};
//static const std::string CONFIG_URL{"http://monitor-config.vivo.lan:8080/api/sdk-config"};      // 正式环境
static const std::string TOKEN{"542171dfaad12ba182774fcb3db745cb"};
static const std::string SPACE_ID{"102"};
static const std::string CONFIG_URL{"http://10.101.18.238:8080/api/sdk-config"};
//static const std::string CONFIG_URL{"http://10.101.193.8:8080/api/sdk-config"};
static const std::string APP_CODE{"general_monitor_example"};

void metricReport()
{
    // 500ms 4个固定第5个构造换一个testB
    std::vector<std::string> vtTest = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t"
            , "u", "v", "w", "x", "y", "z"};
    size_t count(0);
    std::string testE("e");
    const size_t TEST_SIZE(1000);
    const size_t CHANGE_SIZE(TEST_SIZE / 100);
    const size_t SLEEP_TIME(1000000 / TEST_SIZE);
    std::chrono::seconds now = NOW_SECONDS;
    for (size_t i = 0; i < TEST_SIZE; ++i)
    {
        ReportMap mapStat;
        mapStat["testA"] = "a";
        mapStat["testB"] = "b";
        mapStat["testC"] = "c";
        mapStat["testD"] = "d";
        mapStat["testE"] = testE;
        mapStat["testd1"] = "1";
        mapStat["testd2"] = "2";
        mapStat["testd3"] = "3";
        mapStat["testd4"] = "4";
        mapStat["testd5"] = "5";
        // 2、数据上报
        GeneralMonitorReport::report(101185, now.count(), mapStat);
        std::this_thread::sleep_for(std::chrono::microseconds (SLEEP_TIME));
        ++count;

        if (0 == count % CHANGE_SIZE)
        {
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(vtTest.begin(), vtTest.end(), g);
            testE = vtTest[0];
        }
    }

    ReportMap mapStat;
    mapStat["testA"] = "a";
    mapStat["testB"] = "c";
    mapStat["testd1"] = "21";
    // 2、数据上报
    GeneralMonitorReport::report(101185, now.count(), mapStat);
}

void quantileReport()
{
    const size_t SIZE{10};
    //const size_t CHANGE{5};
    std::string ip("10.101.100.160");
    for (size_t i = 0; i < SIZE; ++i)
    {
        ReportMap mapStat;
        mapStat["cmdb_server_node"] = "abc";
        mapStat["cmdb_server_unit_node"] = "def";
        mapStat["terminal_ip"] = ip;
        mapStat["metric_a"] = "a";
        mapStat["metric_b"] = "b";
        double v = 0.000001 * i;
        std::ostringstream buf;
        buf.precision(7);
        buf.setf(std::ios::fixed);
        buf << v;
        mapStat["uv2"] = std::to_string(v);
        LOG(ERROR) << "uv2 test: " << std::setprecision(7) << buf.str() << std::endl;
        // 2、数据上报
        std::chrono::seconds now = NOW_SECONDS;
        GeneralMonitorReport::report(151536, now.count(), mapStat);

        //std::this_thread::sleep_for(std::chrono::microseconds(20));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    /*for (size_t i = 0; i < SIZE; ++i)
    {
        ReportMap mapStat;
        mapStat["cmdb_server_node"] = "abd";
        mapStat["terminal_ip"] = "10.101.100.161";
        mapStat["uv2"] = std::to_string(i + SIZE);
        // 2、数据上报
        std::chrono::seconds now = NOW_SECONDS;
        GeneralMonitorReport::report(151536, now.count(), mapStat);

        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }*/
}

int main(int argv, char **argc)
{
    /* 通用监控使用步骤 */

    /* 1、调用 init 初始化函数，初始化通用监控相关资源。整个上报期间上报前调用一次即可
     * 2、初始化成功后调用 report 上报数据
     * 3、上报数据结束后调用 destroy 回收资源，上报结束后调用一次即可*/

    /* 通用监控使用步骤 */
    std::vector<int> vtRuleId{151497,151536,151646,101229,101185};
    // 1、初始化通用监控sdk。注意configUrl参数需要完整填上路径，如线上域名是http://monitor-config.vivo.lan:8080/
    // ，则configUrl为http://monitor-config.vivo.lan:8080/api/sdk-config
    int ret(GeneralMonitorReport::init(TOKEN, vtRuleId, SPACE_ID, CONFIG_URL, APP_CODE, 0));
    if (MONITOR_CPP_REPORT_SUCCESS == ret)
    {
        // 设置日志等级，如没有需要可不调用，默认开启 error 日志。目前 sdk 只使用了 info、error 两种级别的日志
        //GeneralMonitorReport::setLogLevel(2);
        // 普通指标上报
        //metricReport();
        // 分位指标上报
        quantileReport();

        std::this_thread::sleep_for(std::chrono::seconds(300));
    }

    // 3、退出回收通用监控 sdk 资源
    GeneralMonitorReport::destroy();

    return 0;
}