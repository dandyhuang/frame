//
// Created by 杨民善 on 2020/6/30.
//

#ifndef BLADE_BASE_COMMON_CALC_THREAD_H
#define BLADE_BASE_COMMON_CALC_THREAD_H

#include "../context/monitor_context.h"

namespace vivo_general_monitor_cpp_report
{
    class CalcThread
    {
    public:
        static void calcTask(MonitorContext &ctx);
    };
}

#endif //BLADE_BASE_COMMON_CALC_THREAD_H