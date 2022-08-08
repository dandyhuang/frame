#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_COL_CONCATENATION_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_COL_CONCATENATION_LAYER_H

#include "common.h"
#include "layer.h"
#include "matrix_output.h"

namespace abacus_dnn {

class ColConcatenationLayer : public Layer {
public:
    int input_num() {
        return _inputs.size();
    }
    std::shared_ptr<Layer>& input(int i) {
        return _inputs[i];
    }
    std::vector<std::shared_ptr<Layer>>& inputs() {
        return _inputs;
    }
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void feed_forward() override;
    void back_propagate() override;
private:
    std::vector<std::shared_ptr<Layer>> _inputs;
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_COL_CONCATENATION_LAYER_H
