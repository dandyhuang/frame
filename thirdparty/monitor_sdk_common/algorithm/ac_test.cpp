#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>
#include "thirdparty/monitor_sdk_common/algorithm/KeywordShield.h"

using namespace std;
int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "usage:" << argv[0] << " input_file " << std::endl;
    return -1;
  }
  float time_use = 0;  // 毫秒
  struct timeval start;
  struct timeval end;
  gettimeofday(&start, NULL);

  std::string input_file = argv[1];
  Ahocorasick::CKeywordShield instance;
  if (!instance.Initialize(input_file)) {
    std::cout << "init error" << std::endl;
    return -1;
  }

  gettimeofday(&end, NULL);
  time_use =
      (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;

  std::cout << "time use:" << time_use << std::endl;

  std::string raw_query = "";
  while (true) {
    std::cout << "请输入一行字符串:" << std::endl;
    getline(cin, raw_query);
    if (raw_query.empty()) {
      raw_query = " !深圳宝安区";
    }

    std::string candidate;
    instance.FilterSheild(raw_query, candidate, false);
    std::cout << "hit :" << candidate << std::endl;

    sleep(1);
  }
  return 0;
}