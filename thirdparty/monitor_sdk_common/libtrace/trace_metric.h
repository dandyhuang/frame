//
// Created by 王锦涛 on 2020-2-12.
//

#ifndef COMMON_LIBTRACE_TRACE_METRIC_H
#define COMMON_LIBTRACE_TRACE_METRIC_H

#include <iostream>
#include <string>

namespace common {

class TraceMetric
{
public:
    TraceMetric();
    ~TraceMetric();

    void SetSdkVer(const std::string & sdk_ver);

    void SetTimestamp(const int64_t & timestamp);

    void SetType(const std::string & type);

    void SetComponent(const std::string & component);

    void SetMethodName(const std::string & method_name);

    void SetServiceName(const std::string & service_name);

    void SetDomain(const std::string & domain);

    void SetSign(const std::string & sign);

    void SetSpanId(const std::string & span_id);
private:
    /**
     * required
     * 版本号 默认3.0.0
     */
    std::string sdk_ver_;
    
    /**
     * required
     * 分钟格式化后的时间毫秒数
     */
    int64_t timestamp_;

    /**
     * required
     * 枚举值【CLIENT,SERVER,CONSUMER,PRODUCER】
     */
    std::string type_;

    /**
     * required
     * 枚举值【DUBBO,REDIS,HTTP,CUSTOM,OTHER,MYSQL,RabbitMQ,MONGO,KYLIN】
     */
    std::string component_;

    /**
     * optional
     * 一般为方法名
     */
    std::string method_name_;

    /**
     * optional
     * 一般为接口名
     */
    std::string service_name_;

    /**
     * optional
     * 域名
     */
    std::string domain_;

    /**
     * optional
     * 接口签名
     */
    std::string sign_;

    /**
     * required
     * span id
     */
    std::string span_id_;

    /**
     * required
     * 端信息
     */
    //EndPoint endpoint;

    /**
     * required
     * 附加信息
     */
    //AttachInfo attachInfo;

    /**
     * optional
     * 指标值
     */
    //MValue mvalue;

    /**
     * repeated
     * 扩展信息
     */
    //TagEntry tags;

    /**
     * repeated
     * 扩展维度
     */
    //TagEntry dimension_tags;
};

} //namespace common
#endif //COMMON_LIBTRACE_TRACE_METRIC_H