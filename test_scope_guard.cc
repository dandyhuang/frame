#include <iostream>
#include <map>
#include <string>

#include "scope_guard/scope_guard.h"

int main() {
  int code;
  std::string ctx;
  std::string key;
  std::string dp_rsp;
  dandyhuang::ON_SCOPE_EXIT(([code, ctx, key, &dp_rsp]() {
    std::map<std::string, std::string> map_report;
    map_report["scene_id"] = std::to_string(code);
    map_report["query_dp"] = "1";

  }));
}
