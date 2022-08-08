//
// Created by 王锦涛 on 2020-2-12.
//

#include <iostream>
#include <string>

#include "thirdparty/monitor_sdk_common/libtrace/trace_manager.h"
#include "thirdparty/monitor_sdk_common/system/net/ip_address.h"

using namespace common;

TraceManager::TraceManager() {
    app_name_ = "NULL";
    loc_ = "NULL";
    env_ = ENV_TEST;
    sdk_ver_ = kSDKVER;
    tracers_.clear();
    reporter_ = std::make_shared<TraceReporter>();
}

TraceManager::~TraceManager() {}

void TraceManager::Init(const std::string & app_name,
                        const std::string & loc,
                        const std::string & env) {
    // 基本信息
    app_name_       = app_name;
    loc_            = loc;
    env_            = env;
    sdk_ver_        = kSDKVER;
    sample_rate_    = 1.0;
    // IP地址
    IpAddress address;
    IpAddress::GetFirstLocalAddress(&address);
    ip_ = address.ToString();
    // 定时上报
    StartReporter();
}

void TraceManager::Init(const std::string & app_name,
                        const std::string & loc,
                        const std::string & env,
                        const float & sample_rate,
                        const std::string & ip) {
    // 基本信息
    app_name_       = app_name;
    loc_            = loc;
    env_            = env;
    sdk_ver_        = kSDKVER;
    sample_rate_    = sample_rate;

    IpAddress address;
    if (true == address.Assign(ip)) { // 有效地址
        ip_ = ip;
    } else { // 无效地址，用一个默认地址
        ip_ = "0.0.0.0";
    }
    // 定时上报
    StartReporter();
}

TracerPtr TraceManager::StartTracer() {
    TracerPtr tracer = std::make_shared<TraceTracer>(sample_rate_, app_name_, loc_, env_, sdk_ver_, ip_);
    return tracer;

}

void TraceManager::EndTracer(TracerPtr tracer) {
    std::cout << __FUNCTION__ << std::endl;
    tracers_.push_back(tracer);
}

void TraceManager::StartReporter() {
    std::cout << __FUNCTION__ << std::endl;
    reporter_->Start();
}

std::vector<TracerPtr> TraceManager::PassTracers(){
    std::cout << __FUNCTION__ << std::endl;
    std::vector<TracerPtr> out = tracers_;
    tracers_.clear();
    return out;
}


