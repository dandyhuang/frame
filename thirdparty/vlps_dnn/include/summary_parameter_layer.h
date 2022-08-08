#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_SUMMARY_LAYER_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_SUMMARY_LAYER_H

#include <Eigen/Eigen>
#include "common.h"
#include "matrix_output.h"
#include "layer.h"

namespace abacus_dnn {

class DnnInstance;
class SummaryParameterLayer : public Layer {
public:
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void feed_forward() override;
    void back_propagate() override;
private:
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_SUMMARY_LAYER_H
