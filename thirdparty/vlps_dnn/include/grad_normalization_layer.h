#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_GRAD_NORMALIZATION_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_GRAD_NORMALIZATION_LAYER_H

#include "layer.h"
#include "common.h"
#include "matrix_output.h"

namespace abacus_dnn {

class GradNormalizationLayer : public Layer {
public:
    std::shared_ptr<Layer>& input() {
        return _input;
    }
    std::shared_ptr<Layer>& summary_input() {
        return _sum_input;
    }
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void feed_forward() override;
    void back_propagate() override;
private:
    std::shared_ptr<Layer> _input;
    std::shared_ptr<Layer> _sum_input;

    Eigen::RowVectorXf _means, _scales, _exps;
    abacus::AbacusMatrix _shifts;
    double _squared_sum_epsilon = 1e-4;
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_GRAD_NORMALIZATION_LAYER_H
