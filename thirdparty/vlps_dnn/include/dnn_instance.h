#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_INSTANCE_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_INSTANCE_H

#include <unordered_map>
#include "vlps/common/virtual_object.h"
#include "vlps/common/config.h"
#include "component.h"
#include "layer.h"
#include "vlps/common/failure.h"
#include "vlps/common/macros.h"

#include <glog/logging.h>
#include <string>

namespace abacus_dnn {

class Layer;
class ParamLayer;
class InputLayer;

class DnnInstance : public abacus::VirtualObject {
public:
    static void global_init();

    void initialize(const std::string& path);

    void initialize(abacus::Config conf);

    void feed_forward();

    void back_propagate();

    void finalize();

    int get_tot_param_len() {
        return _tot_param_len;
    }
    bool exist_layer(const std::string& name); 
    std::shared_ptr<Layer> get_layer(const std::string& name); 

    std::vector<std::shared_ptr<Layer>> & get_dnn_layers() {
        return _dnn_layers;
    }

    std::vector<std::shared_ptr<InputLayer>> &get_input_layers() {
        return _input_layers;
    }

    std::vector<std::shared_ptr<ParamLayer>> &get_param_layers() {
        return  _param_layers;
    }
   
    void load_model_from_local(const std::string &param_path);
    abacus::Config get_global_conf() {
        return _global_conf;
    }
    void set_global_conf(abacus::Config conf) {
        _global_conf = conf;
    }

private:

    template<typename LAYERTYPE, typename COMP>
    void load_component(COMP *comp, abacus::Config conf, int *len, const std::string &class_name = "") {
        *len = 0;
        
        for (int i = 0; i < (int)conf.size(); i++) {
            std::shared_ptr<LAYERTYPE> layer;
            layer = global_component_factory().produce<LAYERTYPE>(class_name.empty() ?
                                                                  conf[i]["class"].as<std::string>() :
                                                                  class_name);
            layer->initialize(this, conf[i]);

            {
                abacus::FailureExit on_failure([i]() {
                        MIO_FAILURE_LOG << "name not found for input index[" << i << "]";
                });
                MIO_CHECK(conf[i]["name"].is_defined());
            }
            layer->set_name(conf[i]["name"].as<std::string>());
            comp->push_back(layer);
            _layers.push_back(layer);
            *len += layer->value().size();

            {
                abacus::FailureExit on_dup_failure([i, layer]() {
                        MIO_FAILURE_LOG << "layer " << layer->get_name() << " already exists";
                });
                MIO_CHECK(_components_map.find(layer->get_name()) == _components_map.end());
            }
            _components_map[layer->get_name()] = layer;
        }
    }
    
    int _tot_param_len;
    std::vector<std::shared_ptr<InputLayer>> _input_layers;
    std::vector<std::shared_ptr<ParamLayer>> _param_layers;
    std::vector<std::shared_ptr<Layer>> _dnn_layers;
    std::vector<std::shared_ptr<Layer>> _layers;

    // just for find by name
    std::unordered_map<std::string, std::shared_ptr<Layer>> _components_map;
    abacus::Config _global_conf;
};

inline void get_inputs(DnnInstance *instance, abacus::Config conf,
                       std::vector<std::shared_ptr<Layer>> &inputs) {
     if (conf.is_sequence()) {
        for (size_t i = 0; i < conf.size(); ++i) {
            inputs.push_back(instance->get_layer(conf[i].as<std::string>()));
        }
     } else if (conf.is_scalar()) {
        inputs.push_back(instance->get_layer(conf.as<std::string>()));
     }
}

inline void get_input(DnnInstance *instance, abacus::Config conf,
                      std::shared_ptr<Layer> &input) {
     if (conf.is_scalar()) {
        input = instance->get_layer(conf.as<std::string>());
     }

}

}

#endif
