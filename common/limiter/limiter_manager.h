#pragma once

#include "configs/scene_manager.h"
#include "common/limiter/qps_limiter.h"
#include "common/limiter/token_limiter.h"

namespace rec {
namespace common {

class LimiterManager {
 public:
  static LimiterManager& Instance() {
    static LimiterManager inst;
    return inst;
  }
  LimiterManager() { Init(); }

  void Init() {
    auto& scene_map = g_sceneMgr.scene_map();
    for (auto iter : scene_map) {
      if (qps_cntl_.find(iter.first) != qps_cntl_.end() && iter.second.qps <= 0) {
        continue;
      }
      if (iter.second.limit_type == 0) {
        qps_cntl_[iter.first] = std::make_shared<TokenLimiter>(iter.second.qps, iter.second.precision);
      } else if (iter.second.limit_type == 1) {
        qps_cntl_[iter.first] = std::make_shared<QpsLimiter>(1, iter.second.qps);
        qps_cntl_[iter.first]->UpdateStatus();
      }
    }
  }

  bool Allow(const std::string& url) {
    auto it = qps_cntl_.find(url);
    if (it == qps_cntl_.end()) {
      VLOG_APP(INFO) << " not find url";
      return true;
    }

    if (it->second->Allow()) {
      VLOG_APP(INFO) << "limiter_controller_result: allow"
                     << " qps: " << it->second->GetQps();
      return true;
    } else {
      VLOG_APP(ERROR) << "limiter_controller_result: not allow "
                      << " qps limit: " << it->second->GetQps();
      return false;
    }
    return true;
  }
  float GetQps(const std::string& url) {
    auto it = qps_cntl_.find(url);
    if (it == qps_cntl_.end()) {
      VLOG_APP(INFO) << " not find url";
      return 0;
    }
    return it->second->GetQps();
  }

 private:
  std::unordered_map<std::string, std::shared_ptr<InterfaceLimiter>> qps_cntl_;
};

#define LIMITER_MGR LimiterManager::Instance()

}  // namespace common
}  // namespace rec
