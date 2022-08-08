//
// Created by 王锦涛 on 2020-2-12.
//

#include "thirdparty/monitor_sdk_common/libtrace/trace_metric.h"

using namespace common;

TraceMetric::TraceMetric(){
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

TraceMetric::~TraceMetric(){
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void TraceMetric::SetSdkVer(const std::string & sdk_ver){
    sdk_ver_ = sdk_ver;
}

void TraceMetric::SetTimestamp(const int64_t & timestamp){
    timestamp_ = timestamp;
}

void TraceMetric::SetType(const std::string & type){
    type_ = type;
}

void TraceMetric::SetComponent(const std::string & component){
    component_ = component;
}

void TraceMetric::SetMethodName(const std::string & method_name){
    method_name_ = method_name;
}

void TraceMetric::SetServiceName(const std::string & service_name){
    service_name_ = service_name;
}

void TraceMetric::SetDomain(const std::string & domain){
    domain_ = domain;
}

void TraceMetric::SetSign(const std::string & sign){
    sign_ = sign;
}

void TraceMetric::SetSpanId(const std::string & span_id){
    span_id_ = span_id;
}