//
// Created by 杨民善 on 2020/6/15.
//

#ifndef BLADE_BASE_COMMON_UTIL_H
#define BLADE_BASE_COMMON_UTIL_H

#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <iostream>
#include <regex>
#include <unistd.h>

#include "../log/glog_helper.h"

namespace vivo_bees_bus_cpp_report
{

#define __VIVO_TRY__ try {

#define __VIVO_CATCH_WITH_EXCEPTION__  } catch (std::exception &ex) {         \
    LOG(ERROR) << "exception: " << ex.what() << std::endl;                    \
} catch (...) {                                                               \
    LOG(ERROR) << "unknown exception" << std::endl;                           \
}

#define BEES_NOW_MILLI_SECONDS std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
#define BEES_NOW std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())

    enum VIVO_BEES_CPP_REPORT
    {
        VIVO_BEES_CPP_REPORT_SUCCESS                     = 0,                // 成功
        VIVO_BEES_CPP_REPORT_UNKNOWN                     = 1000,             // 未知错误
        VIVO_BEES_CPP_REPORT_PARAM_EMPTY                 = 1001,             // 参数为空
        VIVO_BEES_CPP_REPORT_GET_CMDB_FAILED             = 1002,             // 获取 cmdb 主机相关信息失败
        VIVO_BEES_CPP_REPORT_GET_CLUSTER_FAILED          = 1003,             // 获取 cluster 列表信息失败
        VIVO_BEES_CPP_REPORT_HAS_INITED                  = 1004,             // 已经初始化过了
        VIVO_BEES_CPP_REPORT_NOT_FINISH_INIT             = 1005,             // 未完成初始化
        VIVO_BEES_CPP_REPORT_CLUSTER_EMPTY               = 1006,             // bees cluster 列表为空
        VIVO_BEES_CPP_REPORT_NEW_THRIFT_CLIENT_FAILED    = 1007,             // thrift 客户端分配失败
    };

    enum VIVO_BEES_CPP_REPORT_METHOD
    {
        VIVO_BEES_CPP_REPORT_METHOD_BEES                 = 0,                // 通过 bees 上报
        VIVO_BEES_CPP_REPORT_METHOD_FLUME                = 1,                // 通过 flume 上报
    };

    static const unsigned int          DEFAULT_INTERVAL{2 * 60};                                       // bees bus cluster 默认轮询间隔
    static const unsigned int          MIN_DEFAULT_INTERVAL{3};                                        // bees bus cluster 最小轮询间隔
    static const unsigned int          DEFAULT_QUICK_INTERVAL{10};                                     // bees bus cluster 请求失败时轮询间隔
    static const int                   DEFAULT_ERROR_CODE{999999};                                     // http 请求默认返回错误码
    static const std::string           BEES_BUS_CPP_REPORT_APP_NAME{"bees_bus_cpp_report"};            // bees manager 心跳上报固定应用名
    static const size_t                MAX_SIZE{2};                                                    // 双缓存大小
    static const std::string           BEES_SDK_VERSION{"1.1.000"};                                    // bees sdk 版本号，每次迭代时或修复bug时更新：主版本号.迭代更新.bug修复
    static const int                   DEFAULT_THRIFT_TIMEOUT{3000};                                   // thrift 默认超时时间，单位是ms
    static const size_t                DEFAULT_THRIFT_RETRY_MAX{3};                                    // 上报失败默认最大重试次数
    static const size_t                MAX_COUNT{3};                                                   // 随机选择 bees cluster 机器台数
    static const size_t                BEES_THRIFT_PORT{4141};                                         // bees bus 默认上报端口
    static const unsigned int          DEFAULT_SECOND{1};                                              // 线程默认睡眠间隔
    static const unsigned int          DEFAULT_FAIL_COUNT{100};                                        // 失败次数阈值
    static const std::string           GET_BUS_LIST{"/sdk/getBusList"};
    static const std::string           TOPIC_KEY{"topic"};
    static const std::string           TAG_KEY{"tag"};
    static const std::string           DATA_TIMESTAMP_KEY{"data_timestamp"};
    static const std::string           LINES_KEY{"lines"};
    static const std::string           IP_KEY{"ip"};
    static const std::string           HOST_NAME_KEY{"hostname"};
    static const std::string           TIMESTAMP_KEY{"timestamp"};
    static const std::string           BYTES_KEY{"bytes"};
    static const std::string           APP_NAME_KEY{"appName"};

    static inline std::string getLocalIp()
    {
        static std::string localIp;
        if (!localIp.empty())
        {
            return localIp;
        }

        FILE *pp = popen("ip a | grep -Ev 'lo:|127.0.0.1|tunl'|awk -F '[ /]+' '/inet / {print $3}'|sort -t. -k 1,3 -k 4.1,4.3 -n | head -1", "r");
        if(nullptr == pp)
        {
            localIp = "0.0.0.0";
            return localIp;
        }

        char ip[16] = {0};
        fgets(ip, sizeof(ip), pp);
        pclose(pp);

        if(strlen(ip) <= 1)
        {
            localIp = "0.0.0.0";
            return localIp;
        }

        localIp = std::string(ip);
        std::regex pattern{" |\n"};
		std::string s = "";
        localIp = std::regex_replace(localIp, pattern, s);

        return localIp;
    }

    static inline std::string getHostName()
    {
        static std::string hostName;
        if (!hostName.empty())
        {
            return hostName;
        }

        char name[256];
        if (0 == gethostname(name, sizeof(name)))
        {
            hostName = name;
        }

        return hostName;
    }
}

#endif //BLADE_BASE_COMMON_UTIL_H