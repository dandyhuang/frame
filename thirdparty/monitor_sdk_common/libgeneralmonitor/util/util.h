//
// Created by 杨民善 on 2020/6/22.
//

#ifndef MONITOR_BLADE_BASE_COMMON_UTIL_H
#define MONITOR_BLADE_BASE_COMMON_UTIL_H

#include <cstdio>
#include <string>
#include <cstring>
#include <ctime>
#include <time.h>

namespace vivo_general_monitor_cpp_report
{

#define NOW_MILLI_SECONDS std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
#define NOW_SECONDS std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())

    enum MONITOR_CPP_REPORT_RESULT
    {
        MONITOR_CPP_REPORT_SUCCESS                    = 0,               // 成功
        MONITOR_CPP_REPORT_PARAM_INVALID              = 2001,            // 参数错误
        MONITOR_CPP_REPORT_HAS_INITED                 = 2002,            // 重复初始化
        MONITOR_CPP_REPORT_CONFIG_URL_EMPTY           = 2003,            // 通用监控配置 url 为空
        MONITOR_CPP_REPORT_CONFIG_FAILED              = 2004,            // 获取通用监控配置失败
        MONITOR_CPP_REPORT_BEES_INIT_FAILED           = 2005,            // 初始化失败
        MONITOR_CPP_REPORT_NOT_FINISH_INITED          = 2006,            // sdk 未始化
        MONITOR_CPP_REPORT_INVALID_RULE_ID            = 2007,            // 非法的 rule id
        MONITOR_CPP_REPORT_FAILED                     = 2008,            // 上报失败（可能是缓存满写入缓存失败了）
        MONITOR_CPP_REPORT_GET_CONFIG_FAILED          = 2009,            // 解析通用监控配置失败
        MONITOR_CPP_REPORT_DATA_EMPTY                 = 2010,            // 上报数据为空
        MONITOR_CPP_REPORT_TASK_NAME_EMPTY            = 2011,            // 任务名为空
    };

    enum VERIFY_RULEID_RESULT
    {
        VERIFY_RULEID_RESULT_SUCCESS = 0,
        VERIFY_RULEID_RESULT_INVALID = 1,
        VERIFY_RULEID_RESULT_EXPIRED = 2,
    };

    static const unsigned int QUICK_INTERVAL{3};

    static const size_t MAX_SIZE{2};

    static const int DEFAULT_ERROR_CODE{999999};

    static const int COLLECT_STATUS_ON{1};
    static const int COLLECT_STATUS_OFF{0};
    static const int ROLLUP_STATUS_ON{1};
    static const int ROLLUP_STATUS_OFF{0};
    static const int MAX_CALC_THREAD_SIZE{256};
    static const int MIN_CALC_THREAD_SIZE{1};
    static const int DEFAULT_QUEUE_SIZE{10000};
    static const int MAX_QUEUE_SIZE{65534};
    static const int DEFAULT_LOOP_INTERVAL{60};
    static const int DEFAULT_REPORT_INTERVAL{10};
    static const int DEFAULT_SECOND{1};
    static const int DEFAULT_MILLI_SECONDS{10};
    static const int FRACTION_UNIT{100};
    static const int PER_SECOND_MILLI{1000};
    static const int DEFAULT_SKETCH_K{128};

    static const std::string           MONITOR_SDK_VERSION{"1.4.000"};       // 监控 sdk 版本号，每次迭代时或修复bug时更新：主版本号.迭代更新.bug修复
    static const std::string           AGGREGATOR_SUM{"sum"};
    static const std::string           AGGREGATOR_MIN{"min"};
    static const std::string           AGGREGATOR_MAX{"max"};
    static const std::string           AGGREGATOR_QUANTILE{"quantile"};
    static const std::string           MONITOR_LANGUAGE_TYPE{"C++"};

    inline long getCollectTime(const long &timestamp, const long &precise)
    {
        return precise > 0 ? timestamp - (timestamp % precise) : timestamp;
    }

    std::string md5str(const std::string &sString);
}

#endif //MONITOR_BLADE_BASE_COMMON_UTIL_H