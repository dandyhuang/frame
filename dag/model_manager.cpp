/***************************************************************************
 *
 * Copyright (c) 2021 dandy.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

 /**
 * @file dag/model_manager.cpp
 * @author dag
 * @date 2021/06/02 15:46:07
 * @version 1.0
 * @brief
 *
 **/

#include <algorithm>
#include "gflags/gflags.h"
#include "common/loghelper.h"
#include "dag/model_manager.h"
#include "boost/filesystem/operations.hpp"

namespace predictor {
namespace model {

DEFINE_int32(model_check_intervals, 30,
"the interval seconds in check model modification time");

bool ModelManager::run() {
    _shd_thread = std::make_shared<std::thread>(&ModelManager::_check, this);
    _shd_thread->detach();
    return true;
}

template<typename ModelClass>
int ModelManager::add_model_sync(const ModelInfo& info) {
    static_assert(std::is_base_of<ModelBase, ModelClass>::value,
    "ModelClass is not the derived class of ModelBase");
    return _add_model<ModelClass>(info);
}

template<typename ModelClass>
int ModelManager::add_model_nowait(const ModelInfo& info) {
    static_assert(std::is_base_of<ModelBase, ModelClass>::value,
    "ModelClass is not the derived class of ModelBase");
    std::lock_guard<std::mutex> lock(_q_mutex);
    _queue.push(info);
    return 0;
}

template<typename ModelClass>
int ModelManager::add_model_if_not_exist(const ModelInfo& info) {
    if (_exist(info.model_name)) {
        return 0;
    }
    return add_model_sync<ModelClass>(info);
}

std::shared_ptr<ModelBase>
ModelManager::get_model(const std::string& name) {
    int idx = _index(name);
    if (idx == -1) {
        return nullptr;
    }
    return _models.at(_current_idxs.at(idx));
}

int ModelManager::_index(const std::string& name) const {
    auto it = _model_2_index_map.find(name);
    if (it != _model_2_index_map.end()) {
        return it->second;
    }
    return -1;
}

bool ModelManager::_exist(const std::string& name) const {
    int idx = _index(name);
    return idx != -1;
}

int ModelManager::_start_index(const std::string& name) const {
    int idx = _index(name);
    if (idx != -1) {
        return idx * num_each_model;
    }
    return -1;
}

int ModelManager::_end_index(const std::string& name) const {
    int idx = _index(name);
    if (idx != -1) {
        return (idx+1) * num_each_model;
    }
    return -1;
}

int ModelManager::_current_index(const std::string& name) const {
    return _current_idxs.at(_index(name));
}

int ModelManager::_next_index(const std::string& name) const {
    int start = _start_index(name);
    int cur = _current_idxs[_index(name)];
    return (cur - start + 1) % num_each_model + start;
}

int ModelManager::_switch_model(const std::string& name) {
    int new_idx = _next_index(name);
    _current_idxs[_index(name)] = new_idx;
    return new_idx;
}

void ModelManager::_update_model(const std::string& name,
        std::shared_ptr<ModelBase> m) {
    int next_idx = _next_index(name);
    _models[next_idx] = m;
    _switch_model(name);
}

// make sure the added model is not exist in model manager.
template<typename ModelClass>
int ModelManager::_add_model(const ModelInfo& info) {
    time_t mtime = boost::filesystem::last_write_time(info.model_path);
    if (mtime == -1) {
        LOG(ERROR) << "get file mtime error, model name: "
                << info.model_name;
    }
    auto m = std::make_shared<ModelClass>(info);
    int init_ret = m->init();
    if (init_ret) {
        LOG(ERROR) << "model init failed.";
        return init_ret;
    }
    int load_ret = m->load();
    if (load_ret) {
        LOG(ERROR) << "load model failed";
        return load_ret;
    }
    {
        // add this model
        _model_2_index_map.insert({info.model_name, _model_infos.size()});
        _model_infos.push_back(info);
        _models.resize(_models.size()+num_each_model, nullptr);
        _current_idxs.push_back(_start_index(info.model_name));
        _models.at(_current_index(info.model_name)) = m;
        _create_funcs.push_back(__create_func<ModelClass>);
    }
    return 0;
}

void ModelManager::_check() {
    while (true) {
        for (auto& info : _model_infos) {
            time_t last_modify_time = boost::filesystem
            ::last_write_time(info.model_path);
            if (last_modify_time == -1) {
                LOG(ERROR) << "get file mtime error, model name: "
                << info.model_name;
                continue;
            }
            // determine if load model
            if (last_modify_time -
            _last_modify_times[_index(info.model_name)] > 60) {
                std::shared_ptr<ModelBase> m
                (_create_funcs[_index(info.model_name)]());
                int init_ret = m->init();
                if (init_ret) {
                    LOG(ERROR) << "model init failed, ret: " << init_ret;
                    continue;
                }
                int load_ret = m->load();
                if (load_ret) {
                    LOG(ERROR) << "model load failed, ret: " << load_ret;
                    continue;
                }
                _update_model(info.model_name, m);
            }
        }
        sleep(FLAGS_model_check_intervals);
    }
}

}  // namespace model
}  // namespace predictor
