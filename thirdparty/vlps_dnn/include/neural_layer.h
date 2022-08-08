#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_NEURAL_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_NEURAL_LAYER_H

#include "layer.h"
#include "common.h"
#include "matrix_output.h"
#include "activation_function.h"
#include "activation_layer.h"
#include "linear_layer.h"
#include "col_concatenation_layer.h"
#include "row_concatenation_layer.h"
#include "multiplication_layer.h"

namespace abacus_dnn {

class NeuralLayer : public Layer {
public:
    int a_input_num() {
        return (int)_a_inputs.size();
    }
    std::shared_ptr<Layer>& a_input(int i) {
        return _a_inputs[i];
    }
    std::vector<std::shared_ptr<Layer>>& a_inputs() {
        return _a_inputs;
    }
    int b_input_num() {
        return (int)_b_inputs.size();
    }
    std::shared_ptr<Layer>& b_input(int i) {
        return _b_inputs[i];
    }
    std::vector<std::shared_ptr<Layer>>& b_inputs() {
        return _b_inputs;
    }
    std::shared_ptr<ActivationFunction>& activation_function() {
        return _act_func;
    }
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void finalize() override;
    void feed_forward() override;
    void back_propagate() override;

private:
    std::vector<std::shared_ptr<Layer>> _a_inputs;
    std::vector<std::shared_ptr<Layer>> _b_inputs;
    std::shared_ptr<ActivationFunction> _act_func;
    abacus::AbacusMatrix _act_grad;

    std::shared_ptr<ColConcatenationLayer> _col_concate_layer;
    std::shared_ptr<RowConcatenationLayer> _row_concate_layer;
    std::shared_ptr<MultiplicationLayer> _mul_layer;
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_NEURAL_LAYER_H
