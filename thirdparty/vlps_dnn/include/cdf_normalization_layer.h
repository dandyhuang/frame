#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_CDF_NORMALIZATION_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_CDF_NORMALIZATION_LAYER_H

#include "layer.h"
#include "common.h"
#include "matrix_output.h"

namespace abacus_dnn {

class CDFNormalizationLayer : public Layer {
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
    int amend(float index, float min_val, float max_val);
private:
    std::shared_ptr<Layer> _input;
    std::shared_ptr<Layer> _sum_input;

    Eigen::RowVectorXf _scales;
    Eigen::RowVectorXf _min_vals;
    Eigen::RowVectorXf _max_vals;
    int _bin_size = -1;
    abacus::AbacusMatrix _indexes;
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_CDF_NORMALIZATION_LAYER_H
