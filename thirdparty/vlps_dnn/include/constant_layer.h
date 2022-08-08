#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_CONSTANT_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_CONSTANT_LAYER_H

#include <Eigen/Eigen>
#include "common.h"
#include "matrix_output.h"
#include "layer.h"
#include "vlps/common/abacus_string.h"

namespace abacus_dnn {

class ConstantLayer : public Layer {
public:
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void feed_forward() override;
    void back_propagate() override;
private:
    int _rows;
    int _cols;
    std::string _initialization_method;
    double _const_val;
    std::vector<double> _user_defined_param;
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_CONSTANT_LAYER_H
