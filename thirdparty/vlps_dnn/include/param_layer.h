#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_PARAM_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_PARAM_LAYER_H

#include <Eigen/Eigen>
#include "common.h"
#include "matrix_output.h"
#include "layer.h"

namespace abacus_dnn {

class ParamLayer : public Layer {
public:
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void dnn_param_initialize(DnnInstance *instance, abacus::Config conf);
    void summary_param_initialize(DnnInstance *instance, abacus::Config conf);
    void grad_norm_param_initialize(DnnInstance *instance, abacus::Config conf);
    void batch_norm_param_initialize(DnnInstance *instance, abacus::Config conf);
    void feed_forward() override;
    void back_propagate() override;
    const std::string& sgd_rule_name() const;
private:
    std::string _sgd_rule_name;
    void common_initialize(abacus::Config conf, std::string _default_sgd_rule_name);
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_PARAM_LAYER_H
