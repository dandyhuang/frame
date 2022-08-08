#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_INITIALIZE_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_INITIALIZE_H

#include "activation_layer.h"
#include "similarity_layer.h"
#include "constant_layer.h"
#include "loss_layer.h"
#include "linear_layer.h"
#include "col_concatenation_layer.h"
#include "row_concatenation_layer.h"
#include "multiplication_layer.h"
#include "neural_layer.h"
#include "grad_normalization_layer.h"
#include "normalization_layer.h"
#include "batch_normalization_layer.h"
#include "layer_normalization_layer.h"
#include "cdf_normalization_layer.h"
#include "col_select_layer.h"
#include "embedding_sum_layer.h"
#include "input_layer.h"
#include "param_layer.h"
#include "summary_parameter_layer.h"
#include "deep_cross_layer.h"

namespace abacus_dnn {

MIO_REGISTER_GLOBAL_INITIALIZER(abacus_dnn_init) {
    static bool inited = false;

    if (inited) {
        return;
    }

    inited = true;

    mkl_set_num_threads(1);
    Eigen::initParallel();

    abacus::Factory<Component>& factory = global_component_factory();

    factory.add<LinearActivationFunction>("linear");
    factory.add<ReluActivationFunction>("relu");
    factory.add<Relu6ActivationFunction>("relu6");
    factory.add<SeluActivationFunction>("selu");
    factory.add<StanhActivationFunction>("stanh");
    factory.add<SoftSignActivationFunction>("soft_sign");
    factory.add<SigmoidActivationFunction>("sig_moid");
    factory.add<ExpActivationFunction>("exp");

    factory.add<CosSimilarityFunction>("cos_sim_func");
    factory.add<InnerProSimilarityFunction>("innerpro_sim_func");

    factory.add<SigmoidLossFunction>("sigmoid");
    factory.add<SoftmaxLossFunction>("softmax");
    factory.add<MSELossFunction>("mse");
    factory.add<RobustMSELossFunction>("robust_mse");
    factory.add<WeightedSigmoidLossFunction>("weighted_sigmoid");
    factory.add<FocalLossFunction>("focal_loss");

    factory.add<ActivationLayer>("act_layer");
    factory.add<LinearLayer>("linear_layer");
    factory.add<ConstantLayer>("constant_layer");
    factory.add<RowConcatenationLayer>("row_concate_layer");
    factory.add<ColConcatenationLayer>("col_concate_layer");
    factory.add<MultiplicationLayer>("mul_layer");
    factory.add<NeuralLayer>("neural_layer");
    factory.add<NormalizationLayer>("normalization_layer");
    factory.add<BatchNormalizationLayer>("batch_normalization_layer");
    factory.add<LayerNormalizationLayer>("layer_normalization_layer");
    factory.add<GradNormalizationLayer>("grad_normalization_layer");
    factory.add<CDFNormalizationLayer>("cdf_normalization_layer");
    factory.add<ColSelectLayer>("col_select_layer");
    factory.add<EmbeddingSumLayer>("embedding_sum_layer");
    factory.add<SimilarityLayer>("similarity_layer");
    factory.add<LossLayer>("loss_layer");
    factory.add<InputLayer>("input_layer");
    factory.add<ParamLayer>("param_layer");
    factory.add<SummaryParameterLayer>("summary_parameter_layer");
    factory.add<DeepCrossLayer>("deep_cross_layer");
}

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_INITIALIZE_H
