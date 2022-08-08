#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_MATRIX_OUTPUT_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_MATRIX_OUTPUT_H

#include <Eigen/Eigen>
#include "output.h"
#include "vlps/common/common.h"

namespace abacus_dnn {

class MatrixOutput : public Output {
public:
    abacus::AbacusMatrix& value() {
        return _value;
    }
    abacus::AbacusMatrix& gradient() {
        return _gradient;
    }
    bool& need_gradient() {
        return _need_gradient;
    }
    bool& has_gradient() {
        return _has_gradient;
    }
private:
    bool _need_gradient = true;
    bool _has_gradient = false;
    abacus::AbacusMatrix _value, _gradient;
};

};
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_MATRIX_OUTPUT_H
