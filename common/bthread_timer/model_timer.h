/***************************************************************************
 *
 * Copyright (c) 2021 Dandyhuang.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

 /**
 * @file timer/model_timer.h
 * @author xiangping.du
 * @date 2021/09/26 10:46:07
 * @version 1.0
 * @brief
 *
 **/

#pragma once

#include <vector>
#include <string>
#include "timer/timer.h"
#include "common/loghelper.h"

namespace predictor {
namespace timer {
class ModelHeartBeatTimer {
 public:
    static ModelHeartBeatTimer& instance() {
        static ModelHeartBeatTimer s;
        return s;
    }
    void init();
    void add_model(const std::string& name) {
        _model_names.push_back(name);
        _timer.schedule_interval_ms(_interval_ms, [name, this] () {
            this->print_model_info(name);
        });
    }
    void print_model_info(const std::string& name);
 private:
    Timer _timer;
    int64_t _interval_ms = 1000;
    std::vector<std::string> _model_names;
};

}  // namespace timer
}  // namespace predictor
