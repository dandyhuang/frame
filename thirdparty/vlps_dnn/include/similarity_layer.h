#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_SIMILARITY_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_SIMILARITY_LAYER_H

#include <Eigen/Eigen>
#include "layer.h"
#include "matrix_output.h"
#include "similarity_function.h"
#include "component.h"
#include "common.h"

namespace abacus_dnn {

class SimilarityLayer : public Layer {
public:
    std::shared_ptr<Layer>& a_input() {
        return _a_input;
    }
    std::shared_ptr<Layer>& b_input() {
        return _b_input;
    }
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void feed_forward() override;
    void back_propagate() override;
private:
    std::shared_ptr<Layer> _a_input;
    std::shared_ptr<Layer> _b_input;
    std::shared_ptr<SimilarityFunction> _sim_func;
    abacus::AbacusMatrix _a_sim_grad;
    abacus::AbacusMatrix _b_sim_grad;
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_SIMILARITY_LAYER_H
