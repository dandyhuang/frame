//
// Created by 王锦涛 on 2020-2-12.
//

#ifndef COMMON_LIBTRACE_TRACE_SPAN_H
#define COMMON_LIBTRACE_TRACE_SPAN_H

#include <tuple>
#include <unordered_map>
#include <vector>

#include "thirdparty/monitor_sdk_common/libtrace/trace_tracer.h"

namespace common {

class TraceTracer;

class TraceSpan {
public:
    friend class TraceTracer;

    TraceSpan(TraceTracer * tracer, const std::string & span_id);

    ~TraceSpan();

    void SetType(const std::string & type);

    void SetComponent(const std::string & component);

    void SetMethodName(const std::string & method_name);

    void SetServiceName(const std::string & service_name);

    void SetDomain(const std::string & domain);

    void SetSign(const std::string & sign);

    void SetStatus(const std::string & status);

    void SetSize(const int32_t & size);

    void SetAnnotation(const std::string & type,
                       const int64_t & timestamp,
                       const std::string & event,
                       const std::string & ex_name);

    void SetTag(const std::string & key,
                const std::string & value);

    void SetEndPoint(const std::string & sip,
                     const int32_t & sport,
                     const std::string & dip,
                     const int32_t & dport);

    /* only for test */
    std::string TestType() {return type_;};
    std::string TestComponent() {return component_;};
    std::string TestMethodName() {return method_name_;};
    std::string TestServiceName() {return service_name_;};
    std::string TestDomain() {return domain_;};
    std::string TestSign() {return sign_;};
    std::string TestStatus() {return status_;};
    int32_t TestSize() {return size_;};

private:
    void SetTraceId(const std::string & trace_id);

    void SetId(const std::string & id);

    void SetSdkVer(const std::string & sdk_ver);

    void SetStart();

    void SetEnd();

    void SetAttachInfo(const std::string & app_name,
                       const std::string & loc,
                       const std::string & env);

    /**
     * 该span所属于的tracer
     */
    TraceTracer * tracer_;

    /**
     * required
     * trace id
     */
    std::string trace_id_;

    /*
     * required
     * span id
     */
    std::string id_;

    /*
     * required
     * 版本号，默认3.0.0
     */
    std::string sdk_ver_;

    /*
     * required
     * span开始时间毫秒数
     */
    int64_t start_;

    /*
     * required
     * span结束时间毫秒数
     */
    int64_t end_;

    /*
     * required
     * CLIENT,SERVER,CONSUMER,PRODUCER
     */
    std::string type_;

    /*
     * required
     * DUBBO,REDIS,HTTP,CUSTOM, TARS
     */
    std::string component_;

    /*
     * required
     * 一般为方法名
     */
    std::string method_name_;

    /*
     * optional
     * 服务名
     */
    std::string service_name_;

    /*
     * optional
     * 域名
     */
    std::string domain_;

    /*
     * optional
     * 签名
     */
    std::string sign_;

    /*
     * required
     * 状态码 枚举值【SUCCESS, TIMEOUT, FAILED ,EXCEPTION】
     */
    std::string status_;

    /*
     * required
     * 返回数据大小
     */
    int32_t size_;

    /*
     * required AttachInfo          附加信息
     * 1. optional string app_name  应用名
     * 2. optional string loc       机房
     * 3. optional string env       环境
     */
    std::string app_name_;
    std::string loc_;
    std::string env_;

    /*
     * repeated Annotation          事件标注
     * required string type         枚举类型【THROWABLE, SQL, ERROR_LOG】
     * required int64 timestamp     时间戳
     * required string event        事件
     * optional string ex_name      名字
     */
    std::vector<std::tuple<std::string, int64_t, std::string, std::string>> annotations_;


    /*
     * repeated Tag                 标签
     * required string k            key
     * optional string v            value
     */
    std::unordered_map<std::string, std::string> tags_;

    /*
     * required EndPoint            端信息
     * 1. required string sip       源ip
     * 2. optional int32 sport      源端口
     * 3. required string dip       目的ip
     * 4. optional int32 dport      目的端口
     */
    std::string sip_;
    int32_t sport_;
    std::string dip_;
    int32_t dport_;
};

} //namespace common
#endif //COMMON_LIBTRACE_TRACE_SPAN_H