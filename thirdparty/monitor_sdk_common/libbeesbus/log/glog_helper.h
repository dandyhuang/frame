//
// Created by 杨民善 on 2020/6/18.
//

#ifndef BLADE_BASE_COMMON_GLOG_HELPER_H
#define BLADE_BASE_COMMON_GLOG_HELPER_H

#include "thirdparty/glog/logging.h"
#include "thirdparty/glog/raw_logging.h"

namespace vivo_bees_bus_cpp_report
{
    #define GLOG_DIR                   "glog"
    enum VIVO_BEES_GLOG_LEVEL
    {
        VIVO_BEES_GLOG_LEVEL_INFO                        = 0,                // INFO 级别日志
        VIVO_BEES_GLOG_LEVEL_WARNING                     = 1,                // WARNING 级别日志
        VIVO_BEES_GLOG_LEVEL_ERROR                       = 2,                // ERROR 级别日志
        VIVO_BEES_GLOG_LEVEL_FATAL                       = 3,                // FATAL 级别日志，最严重的日志，输出之后会中止程序
    };

    class GlogHelper
    {
    public:
        GlogHelper();
        virtual ~GlogHelper();
        static void signalHandle(const char* data, int size);
        static void init(char* program, const int &logLevel = VIVO_BEES_GLOG_LEVEL_ERROR);
        static void setGlogLevel(const int &logLevel = VIVO_BEES_GLOG_LEVEL_ERROR);
        static void destroy();
    };
}


#endif //BLADE_BASE_COMMON_GLOG_HELPER_H
