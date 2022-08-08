#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_INPUT_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_INPUT_LAYER_H

#include <Eigen/Eigen>
#include "common.h"
#include "matrix_output.h"
#include "layer.h"

namespace abacus_dnn {

class InputLayer : public Layer {
public:
    void load_config(abacus::Config conf);
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void feed_forward() override;
    void back_propagate() override;
    const abacus::Config get_config() const {
        return _config;
    }
private:
    abacus::Config _config;
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_INPUT_LAYER_H
