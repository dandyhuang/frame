//
// Created by 王锦涛 on 2020-2-12.
//

#ifndef COMMON_LIBTRACE_TRACE_MANAGER_H
#define COMMON_LIBTRACE_TRACE_MANAGER_H

#include <memory>
#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "thirdparty/monitor_sdk_common/libtrace/trace_reporter.h"
#include "thirdparty/monitor_sdk_common/libtrace/trace_span.h"
#include "thirdparty/monitor_sdk_common/libtrace/trace_tracer.h"

namespace common {

typedef std::shared_ptr<TraceTracer>    TracerPtr;
typedef std::shared_ptr<TraceSpan>      SpanPtr;
typedef std::shared_ptr<TraceReporter>  ReporterPtr;

static const std::string ENV_PRD    = "p";      //线上
static const std::string ENV_PRE    = "r";      //预上线
static const std::string ENV_TEST   = "t";      //测试
static const std::string ENV_DEV    = "d";      //开发
static const std::string ENV_PRESS  = "s";      //压测
static const std::string ENV_OTHER  = "u";      //其他

static const std::string kROOT      = "1";      //根结点
static const std::string kSDKVER    = "1.0.0";  //sdk版本号


class TraceManager : public SingletonBase<TraceManager> {
    friend class SingletonBase<TraceManager>;

public:
    friend class TraceReporter;

    /**
     * 初始化
     * 应用名，机房，环境，采样率，IP地址
     */
    void Init(const std::string & app_name, const std::string & loc, const std::string & env);
    void Init(const std::string & app_name, const std::string & loc, const std::string & env, const float & sample_rate, const std::string & ip);

    /**
     * 开始tracer
     */
    TracerPtr StartTracer();

    /**
     * 结束tracer
     */
    void EndTracer(TracerPtr tracer);

private:
    TraceManager();

    ~TraceManager();

    /**
     * 定时上报
     */
    void StartReporter();

    /**
     *  获取tracers_
     */
    std::vector<TracerPtr> PassTracers();

    /**
     * 采样率，默认为1
     */
    float sample_rate_;

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
     * 定时上报类实例
     */
    ReporterPtr reporter_;

    /**
     * 存储tracer
     */
    std::vector<TracerPtr> tracers_;
};


} //namespace common
#endif //COMMON_LIBTRACE_TRACE_MANAGER_H