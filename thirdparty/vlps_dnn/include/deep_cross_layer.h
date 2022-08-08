#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_DEEP_CROSS_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_DEEP_CROSS_LAYER_H

#include "layer.h"
#include "common.h"
#include "matrix_output.h"

namespace abacus_dnn {

class DeepCrossLayer : public Layer {
public:
    std::vector<std::shared_ptr<Layer>>& w_inputs() {
        return _w_inputs;
    }
    std::vector<std::shared_ptr<Layer>>& b_inputs() {
        return _b_inputs;
    }
    std::shared_ptr<Layer>& x0_input() {
        return _x0_input;
    }
    std::vector<Eigen::MatrixXf>& xs() {
        return _xs;
    }
    std::vector<Eigen::MatrixXf>& xj_wjs() {
        return _xj_wjs;
    }
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void feed_forward() override;
    void back_propagate() override;
private:
    std::vector<std::shared_ptr<Layer>> _w_inputs;
    std::vector<std::shared_ptr<Layer>> _b_inputs;
    std::shared_ptr<Layer> _x0_input;
    std::vector<Eigen::MatrixXf> _xs;
    std::vector<Eigen::MatrixXf> _xj_wjs;
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_MULTIPLICATION_LAYER_H
