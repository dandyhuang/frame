//
// Created by 王锦涛 on 2020-2-12.
//

#ifndef COMMON_LIBTRACE_TRACE_TRACER_H
#define COMMON_LIBTRACE_TRACE_TRACER_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/libtrace/trace_span.h"
#include "thirdparty/monitor_sdk_common/libtrace/trace_util.h"
#include "thirdparty/monitor_sdk_common/system/net/ip_address.h"

namespace common {

class TraceSpan;

class TraceTracer {
public:
    friend class TraceSpan;

    TraceTracer(const float & sample_rate,
                const std::string & app_name,
                const std::string & loc,
                const std::string & env,
                const std::string & sdk_ver,
                const std::string & ip);

    ~TraceTracer();

    /**
     * 开始span
     */
    std::shared_ptr<TraceSpan> StartSpan(const std::string & span_id);

    /**
     * 结束span
     */
    void EndSpan(std::shared_ptr<TraceSpan> span);

    /**
     * 设置采样率
     */
    void SetSampleRate(const float & sample_rate);

    /* only for test */
    size_t TestSpanSize() {return spans_.size();};
    float TestSampleRate() {return sample_rate_;};

private:
    /**
     * 采样率，默认为1
     */
    float sample_rate_;

    /**
     * trace id
     */
    std::string trace_id_;

    /**
     * 应用名
     */
    std::string app_name_;

    /**
     * 机房名
     */
    std::string loc_;

    /**
     * 环境
     */
    std::string env_;

    /**
     * sdk版本号 默认为1.0.0
     */
    std::string sdk_ver_;

    /**
     * 本地ip地址
     */
    std::string ip_;

    /**
     *  用于储存当前tacer所涵盖的span
     */
    std::vector<std::shared_ptr<TraceSpan>> spans_;
};

} //namespace common
#endif //COMMON_LIBTRACE_TRACE_TRACER_H