#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_LAYER_NORMALIZATION_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_LAYER_NORMALIZATION_LAYER_H

#include "layer.h"
#include "common.h"
#include "matrix_output.h"

namespace abacus_dnn {

class LayerNormalizationLayer : public Layer {
public:
    std::shared_ptr<Layer>& input() {
        return _input;
    }
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void feed_forward() override;
    void back_propagate() override;
private:
    std::shared_ptr<Layer> _input;
    std::vector<double> _means;
    std::vector<double> _scales;
    double _squared_sum_epsilon = 1e-4;
    bool _enable_mean_var_grad = true;
    std::vector<double> _mean_grad;
    std::vector<double> _scales_grad;
    
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_LAYER_NORMALIZATION_LAYER_H
