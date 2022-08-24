/***************************************************************************
 *
 * Copyright (c) 2021 Vivo.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

 /**
 * @file timer/model_timer.cpp
 * @author xiangping.du
 * @date 2021/09/26 10:46:07
 * @version 1.0
 * @brief
 *
 **/

#include "timer/model_timer.h"
#include "model_manager/model_manager.h"
#include "transactions/monitor/model_monitor_info.h"

namespace predictor {
namespace timer {

void ModelHeartBeatTimer::init() {
    _timer.run();
    zeus::global::ModelManager::instance()
        .for_each_model([this] (const std::string& name) {
        this->add_model(name);
    });
}

void ModelHeartBeatTimer::print_model_info(const std::string& name) {
    auto m = zeus::global::ModelManager::instance().get(name);
    if (m == nullptr) {
        VLOG_APP(ERROR) << "model is nullptr";
        return;
    }
    predictor::monitor::ModelMonitorInfo info(name, m.get());
    VLOG_MODEL(INFO) << info.to_json_str();
}

}  // namespace timer
}  // namespace predictor