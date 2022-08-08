// @copyright vivo
// @author yuchengzhen
// 2019.07.02
#pragma once

#include <string>
#include <vector>

class PbLoader {
 public:
  PbLoader() {}
  ~PbLoader();

  int Init(const std::string &proto_file, const std::vector<std::string> &path_list);
  int Json2PbBinary(const std::string &msg_name, const std::string &json_str, std::string &out) const;  // NOLINT
  int PbBinary2Json(const std::string &msg_name, const std::string &pb_binary, std::string &out) const;  // NOLINT
  int PbBinary2FullJson(const std::string &msg_name, const std::string &pb_binary, std::string &out) const;  // NOLINT
  int PbDefalut2FullJson(const std::string &msg_name, std::string &out) const;  // NOLINT

 private:
  void *importer_{nullptr};
};
