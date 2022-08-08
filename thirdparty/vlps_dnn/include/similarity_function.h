#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_SIMILARITY_FUNCTION_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_SIMILARITY_FUNCTION_H

#include <mkl.h>
#include <mkl_cblas.h>
#include <Eigen/Eigen>
#include "component.h"
#include "vlps/common/common.h"

namespace abacus_dnn {

class SimilarityFunction : public Component {
public:
    virtual void compute(const abacus::AbacusMatrix& a_input, abacus::AbacusMatrix& a_grads,
            bool a_need_gradient, const abacus::AbacusMatrix& b_input,
            abacus::AbacusMatrix& b_grads, bool b_need_gradient,
            abacus::AbacusMatrix& outs) = 0;
};

class CosSimilarityFunction : public SimilarityFunction {
public:
    void compute(const abacus::AbacusMatrix& a_input, abacus::AbacusMatrix& a_grads,
            bool a_eed_gradient, const abacus::AbacusMatrix& b_input,
            abacus::AbacusMatrix& b_grads, bool b_need_gradient, abacus::AbacusMatrix& outs) override;
};

class InnerProSimilarityFunction : public SimilarityFunction {
public:
    void compute(const abacus::AbacusMatrix& a_input, abacus::AbacusMatrix& a_grads,
            bool a_need_gradient, const abacus::AbacusMatrix& b_input,
            abacus::AbacusMatrix& b_grads, bool b_need_gradient, abacus::AbacusMatrix& outs) override;
};

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_SIMILARITY_FUNCTION_H
