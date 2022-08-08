#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_EMBEDDING_SUM_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_EMBEDDING_SUM_LAYER_H

#include "layer.h"
#include "common.h"
#include "matrix_output.h"

namespace abacus_dnn {

class EmbeddingSumLayer : public Layer {
public:
    std::shared_ptr<Layer>& input() {
        return _input;
    }
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void feed_forward() override;
    void back_propagate() override;
private:
    std::shared_ptr<Layer> _input;
    int _dim;
};
}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_EMBEDDING_SUM_LAYER_H
