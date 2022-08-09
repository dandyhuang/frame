#pragma once
#include <string>
#include <vector>

#include "dag/graph.h"
#include "dag/graph_manager.h"

namespace dag {

class dag_001 : public dag::Node {
 public:
  dag_001(const string& serviceName) : dag::Node(serviceName){};
  dag_001() : dag::Node(){};
  virtual ~dag_001(){};
  int do_service(std::shared_ptr<frame::Context> context) noexcept override ;
  virtual bool skip(std::shared_ptr<frame::Context> context) override;

 private:
  int32_t channel_id_ = 10002;                       //  通道号
  std::string channel_name_ = "high_quality_10002";  //  通道名
  int32_t channel_weight_ = 0;                       //  通道权重
};

}  // namespace dag