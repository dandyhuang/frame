#pragma once
#include <brpc/channel.h>

#include <string>
#include <unordered_map>

#include "brpc/callback.h"
#include "common/loghelper.h"

namespace dag {
class GtransportManager {
 public:
  static GtransportManager& instance();

  void init(const std::string& conf_path) noexcept;

  std::shared_ptr<brpc::Channel> get_transport(const std::string& transport_name);

 private:
  std::unordered_map<std::string, std::shared_ptr<brpc::Channel>> channel_map;
};
}  // end of namespace dag
