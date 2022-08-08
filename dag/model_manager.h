/***************************************************************************
 *
 * Copyright (c) 2021 Vivo.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

 /**
 * @file dag/model_manager.h
 * @author dag
 * @date 2021/06/02 15:46:07
 * @version $Revision$
 * @brief
 *
 **/
#pragma once

#include <vector>
#include <queue>
#include <string>
#include <memory>
#include <mutex>  // NOLINT [build/c++11]
#include <unordered_map>
#include <thread>  // NOLINT [build/c++11]
#include "context/base.h"
#include "butil/containers/flat_map.h"

namespace predictor {
namespace model {

struct ModelInput {
};

struct ModelOutput {
};

struct ModelInfo {
    std::string model_name;
    std::string model_path;
};

class ModelBase {
 public:
    explicit ModelBase(ModelInfo);
    virtual ~ModelBase() = default;
    virtual int init() = 0;
    // model predict
    virtual std::shared_ptr<ModelOutput>
    predict(const std::shared_ptr<ModelInput>& input) = 0;
    // load model, return 0 if success, otherwise a positive value.
    virtual int load() = 0;
    // get embedding from model
    virtual std::shared_ptr<butil::FlatMap<HashType, EmbeddingRecord_float>>
    get_embedding(const std::vector<HashType>& hashs) {
        return nullptr;
    }
};

template<class T>
T* __create_func() {
    return new T();
}

class ModelManager {
using CreateFunc = ModelBase* (*)();
constexpr static int num_each_model = 2;

 private:
    ModelManager() = default;
    ~ModelManager() = default;

 public:
    static ModelManager& instance() {
        static ModelManager m;
        return m;
    }

    // call after all models added.
    bool run();

    template<typename ModelClass>
    int add_model_sync(const ModelInfo&);

    template<typename ModelClass>
    int add_model_nowait(const ModelInfo&);

    template<typename ModelClass>
    int add_model_if_not_exist(const ModelInfo&);

    std::shared_ptr<ModelBase> get_model(const std::string&);

 private:
    int _index(const std::string&) const;
    bool _exist(const std::string&) const;
    int _start_index(const std::string&) const;
    int _end_index(const std::string&) const;
    int _current_index(const std::string&) const;
    int _next_index(const std::string&) const;
    int _switch_model(const std::string& name);
    void _update_model(const std::string& name, std::shared_ptr<ModelBase>);
    template<typename ModelClass>
    int _add_model(const ModelInfo&);

    void _check();

 private:
    // model container, num_each_model for each model.
    std::vector<std::shared_ptr<ModelBase>> _models;
    // current index in _models for each model.
    std::vector<int> _current_idxs;
    // store each model.
    std::vector<ModelInfo> _model_infos;
    // model name -> index
    std::unordered_map<std::string, int> _model_2_index_map;
    // the last modify time for each model
    std::vector<time_t> _last_modify_times;
    std::vector<CreateFunc> _create_funcs;
    // queue for async add model
    std::queue<ModelInfo> _queue;
    std::mutex _q_mutex;
    boost::shared_mutex _shd_mutex;
    std::shared_ptr<std::thread> _shd_thread;
};

}  // namespace model
}  // namespace predictor
