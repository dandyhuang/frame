#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_BATCH_NORMALIZATION_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_BATCH_NORMALIZATION_LAYER_H

#include "layer.h"
#include "common.h"
#include "matrix_output.h"

namespace abacus_dnn {

class BatchNormalizationLayer : public Layer {
public:
    std::shared_ptr<Layer>& input() {
        return _input;
    }
    std::shared_ptr<Layer>& param_input() {
        return _param_input;
    }
    double& epsilon() {
        return _epsilon;
    }
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void feed_forward() override;
    void back_propagate() override;

private:
    std::shared_ptr<Layer> _input;
    std::shared_ptr<Layer> _param_input;
    std::shared_ptr<Layer> _global_stats;
    double _epsilon = 1e-4;

    Eigen::Array<float, 1, Eigen::Dynamic> _means, _variances, _scales, _mean_grad, _variance_grad;
    Eigen::ArrayXXf _normalized_val, _normalized_grad;

    bool _use_global_stats = false;
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_BATCH_NORMALIZATION_LAYER_H
