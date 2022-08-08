// Authors: weitao.zhang@vivo.com

#ifndef REC_MODEL_H_
#define REC_MODEL_H_
#include "proto/recommend.pb.h"
#include "compute_server.pb.h"
#include "model/ml/ml_model.h"
#include "common/type.h"


namespace Rec {
namespace Model {
class Model {
    public:
        virtual void set_model_path(const string& modelPath) = 0;
        virtual int LoadModel(const string& modelPath, bool is_auto_load = false) = 0;
        virtual int ReLoadModel(const string& modelPath) = 0;
        virtual int init(const string& modelPath) = 0;
        virtual Model* get_inner_model() {return nullptr;}
        virtual ML::ModelPredictStatus Predict(vmic::feature::Response & response, 
                std::unordered_map<uint64_t, rec::PredictInfo> &result,
                std::unordered_map<std::string, rec::PredictInfo> &result_lr,
                int target, std::shared_ptr<ML::ScoreContext> score_context) = 0;
};

} // namespace Model
} // namespace Rec

#endif // REC_MODEL_H_
