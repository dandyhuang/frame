#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_LOSS_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_LOSS_LAYER_H

#include <Eigen/Eigen>
#include "layer.h"
#include "matrix_output.h"
#include "loss_function.h"
#include "component.h"
#include "common.h"

namespace abacus_dnn {

class LossLayer : public Layer {
public:
    std::shared_ptr<Layer>& input() {
        return _input;
    }
    std::shared_ptr<Layer>& target() {
        return _target;
    }
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void feed_forward() override;
    void back_propagate() override;
private:
    std::shared_ptr<Layer> _input;
    std::shared_ptr<Layer> _target;
    std::shared_ptr<LossFunction> _loss_func;
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_LOSS_LAYER_H
