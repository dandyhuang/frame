#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_ACTIVATION_FUNCTION_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_ACTIVATION_FUNCTION_H

#include <mkl.h>
#include <mkl_cblas.h>
#include "component.h"

namespace abacus_dnn {

class ActivationFunction : public Component {
public:
    virtual void init(abacus::Config conf) = 0; 
    virtual void compute(size_t n, const float* ins, float* outs, float* grads) = 0;
};

class LinearActivationFunction : public ActivationFunction {
public:
    void init(abacus::Config conf) override;
    void compute(size_t n, const float* ins, float* outs, float* grads) override;
};

class SoftSignActivationFunction : public ActivationFunction {
public:
    void init(abacus::Config conf) override;
    void compute(size_t n, const float* ins, float* outs, float* grads) override;
private:
    float _softsign_a;
    float _softsign_b;
};

class SeluActivationFunction : public ActivationFunction {
public:
    void init(abacus::Config conf) override;
    void compute(size_t n, const float* ins, float* outs, float* grads) override;
private:
    float _lambda = 1.0507;
    float _alpha = 1.6732;
    float _selu_bound;
    bool _enable_bound = false;
};

class ReluActivationFunction : public ActivationFunction {
public:
    void init(abacus::Config conf) override;
    void compute(size_t n, const float* ins, float* outs, float* grads) override;
private:
    float _relu_bound;
    bool _enable_bound = false;
};

class Relu6ActivationFunction : public ActivationFunction {
public:
    void init(abacus::Config conf) override;
    void compute(size_t n, const float* ins, float* outs, float* grads) override;
private:
    ReluActivationFunction _relu_func;
};

class StanhActivationFunction : public ActivationFunction {
public:
    void init(abacus::Config conf) override;
    void compute(size_t n, const float* ins, float* outs, float* grads) override;
private:
    float _stanh_a;
    float _stanh_b;
};

class SigmoidActivationFunction : public ActivationFunction {
public:
    void init(abacus::Config conf) override;
    void compute(size_t n, const float* ins, float* outs, float* grads) override;
private:
    bool _bound_value;
    const float up_bound = 15.0f;
    const float lower_bound = -15.0f;
};

class ExpActivationFunction : public ActivationFunction {
public:
    void init(abacus::Config conf) override;
    void compute(size_t n, const float* ins, float* outs, float* grads) override;
};
}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_ACTIVATION_FUNCTION_H
