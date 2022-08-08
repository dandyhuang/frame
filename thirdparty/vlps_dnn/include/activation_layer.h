#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_ACTIVATION_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_ACTIVATION_LAYER_H

#include <Eigen/Eigen>
#include "layer.h"
#include "matrix_output.h"
#include "activation_function.h"
#include "component.h"
#include "common.h"

namespace abacus_dnn {

class ActivationLayer : public Layer {
public:
    std::shared_ptr<Layer>& input() {
        return _input;
    }
    std::shared_ptr<ActivationFunction>& activation_function() {
        return _act_func;
    }
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void feed_forward() override;
    void back_propagate() override;
private:
    std::shared_ptr<Layer> _input;
    std::shared_ptr<ActivationFunction> _act_func;
    abacus::AbacusMatrix _act_grad;
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_ACTIVATION_LAYER_H
