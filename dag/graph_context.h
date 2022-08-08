#pragma once
#include <atomic>
#include <memory>
#include <unordered_map>
namespace dag {
class Node;
class GraphContext {
 public:
  std::unordered_map<Node*, std::shared_ptr<std::atomic<int>>> node_input_num_map;
  virtual int Init() = 0;
  virtual void clear() = 0;
};
}  // namespace dag
