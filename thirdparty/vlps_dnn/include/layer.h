#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_LAYER_H

#include "matrix_output.h"
#include "component.h"
#include "vlps/common/config.h"
#include <memory>
#include <vector>
#include <Eigen/Eigen>


namespace abacus_dnn {

class DnnInstance;
class Layer : public Component {
public:
    virtual void initialize(DnnInstance *instance, abacus::Config conf) {
    }
    virtual void finalize() {
    }
    virtual void feed_forward() {
    }
    virtual void back_propagate() {
    }
    virtual abacus::AbacusMatrix& value() {
        return _output->value();
    }
    virtual abacus::AbacusMatrix& gradient() {
        return _output->gradient();
    }
    virtual bool& need_gradient() {
        return _output->need_gradient();
    }
    virtual bool& has_gradient() {
        return _output->has_gradient();
    }
protected:
    std::shared_ptr<MatrixOutput> _output = std::make_shared<MatrixOutput>();
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_LAYER_H
