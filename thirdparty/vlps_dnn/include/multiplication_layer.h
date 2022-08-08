#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_MULTIPLICATION_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_MULTIPLICATION_LAYER_H

#include "layer.h"
#include "common.h"
#include "matrix_output.h"

namespace abacus_dnn {

class MultiplicationLayer : public Layer {
public:
    std::shared_ptr<Layer>& a_input() {
        return _a_input;
    }
    std::shared_ptr<Layer>& b_input() {
        return _b_input;
    }
    bool& a_transpose() {
        return _a_trans;
    }
    bool& b_transpose() {
        return _b_trans;
    }
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void feed_forward() override;
    void back_propagate() override;
private:
    std::shared_ptr<Layer> _a_input;
    std::shared_ptr<Layer> _b_input;
    bool _a_trans = false;
    bool _b_trans = false;
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_MULTIPLICATION_LAYER_H
