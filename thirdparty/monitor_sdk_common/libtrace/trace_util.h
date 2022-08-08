//
// Created by 王锦涛 on 2020-2-12.
//

#ifndef COMMON_LIBTRACE_TRACE_UTIL_H
#define COMMON_LIBTRACE_TRACE_UTIL_H

#include <atomic>
#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>


#include "thirdparty/monitor_sdk_common/base/singleton.h"

namespace common {

static const int CYCLE = 10000;

class TraceID : public SingletonBase<TraceID> {
    friend class SingletonBase<TraceID>;

public:
    std::string GenTraceId(const std::string & env, const std::string & ip_str);

private:
    TraceID();

    ~TraceID() {};

    std::string EncodeIp(const std::string & ip_str);

    std::atomic<int> serial_number;
};

class TraceTime : public SingletonBase<TraceTime> {
    friend class SingletonBase<TraceTime>;

public:
    int64_t GetTimeS();

    int64_t GetTimeMS();

private:
    TraceTime() {};

    ~TraceTime() {};
};

} //namespace common
#endif //COMMON_LIBTRACE_TRACE_UTIL_H