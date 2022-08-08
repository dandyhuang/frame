#ifndef BAIDU_FCR_MODEL_ABACUS_LEARNER_LOSS_FUNCTION_H
#define BAIDU_FCR_MODEL_ABACUS_LEARNER_LOSS_FUNCTION_H

#include <mkl.h>
#include <mkl_cblas.h>
#include <Eigen/Eigen>
#include "component.h"
#include "layer.h"
#include "dnn_instance.h"

namespace abacus_dnn {

const float soft_max_up_bound = 15.0f;

const float soft_max_lower_bound = -15.0f;

const float min_precision = 1e-6f;	

class LossFunction : public Component {
public:
    virtual void initialize(DnnInstance *instance, abacus::Config conf) {
    }
    virtual void compute(const abacus::AbacusMatrix& inputs, const abacus::AbacusMatrix& labels, abacus::AbacusMatrix& preds, abacus::AbacusMatrix& gradient) = 0;
};

class MSELossFunction : public LossFunction {
public:
    void compute(const abacus::AbacusMatrix& inputs, const abacus::AbacusMatrix& labels, abacus::AbacusMatrix& preds, abacus::AbacusMatrix& gradient) override;
};

class RobustMSELossFunction : public LossFunction {
public:
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void compute(const abacus::AbacusMatrix& inputs, const abacus::AbacusMatrix& labels, abacus::AbacusMatrix& preds, abacus::AbacusMatrix& gradient) override;
private:
    double _gradient_bound;
};

class SigmoidLossFunction : public LossFunction {
public:
    void compute(const abacus::AbacusMatrix& inputs, const abacus::AbacusMatrix& labels, abacus::AbacusMatrix& preds, abacus::AbacusMatrix& gradient) override;
};


class FocalLossFunction : public LossFunction {
public:
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void compute(const abacus::AbacusMatrix& inputs, const abacus::AbacusMatrix& labels, abacus::AbacusMatrix& preds, abacus::AbacusMatrix& gradient) override;
private:
    float _gamma;
    float _alpha;
};

class WeightedSigmoidLossFunction : public LossFunction {
public:
    void initialize(DnnInstance *instance, abacus::Config conf) override;
    void compute(const abacus::AbacusMatrix& inputs, const abacus::AbacusMatrix& labels, abacus::AbacusMatrix& preds, abacus::AbacusMatrix& gradient) override;
private:
    std::shared_ptr<Layer> _ins_sample_weight;
};

class SoftmaxLossFunction : public LossFunction {
public:
    void compute(const abacus::AbacusMatrix& inputs, const abacus::AbacusMatrix& labels, abacus::AbacusMatrix& preds, abacus::AbacusMatrix& gradient) override;
};

// TODO
/*class RankLossFunction : public LossFunction { 
    void compute(const abacus::AbacusMatrix& inputs, const abacus::AbacusMatrix& labels, abacus::AbacusMatrix& preds, abacus::AbacusMatrix& gradient, int output_dim ) override;
};*/

// TODO
/*class HingeFunction : public LossFunction { 
    void compute(const abacus::AbacusMatrix& inputs, const abacus::AbacusMatrix& labels, abacus::AbacusMatrix& preds, abacus::AbacusMatrix& gradient, int output_dim ) override;
};*/

}
#endif //BAIDU_FCR_MODEL_ABACUS_LEARNER_LOSS_FUNCTION_H
