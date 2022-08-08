// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: libin <langman.bin@gmail.com>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <vector>
#include "thirdparty/monitor_sdk_common/oss/oss_agent_api.h"
#include "thirdparty/monitor_sdk_common/oss/oss_api.h"
using std::vector;
using std::endl;
using std::cout;
using std::thread;

#define CHECK_RET(ret)                    \
  if (ret) {                              \
    cout << "error: ret " << ret << endl; \
    exit(-1);                             \
  }

void func() {
  OssAttrInc(100, 0, 200);
  OssAttrInc(100, 0, 1);
  OssAttrInc(105, 0, 100);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    cout << "usage: " << argv[0] << "thread_num" << endl;
    return -1;
  }
  OssAttrInc(100, 0, 0);

  int cnt = atoi(argv[1]);
  vector<thread*> vec;
  vec.reserve(cnt);
  for (int i = 0; i < cnt; ++i) {
    vec.push_back(new thread(func));
  }
  for (auto& t : vec) t->join();

  vector<OssInfo> oss_vec;
  int ret = SwitchAndGetAttrData(oss_vec);
  CHECK_RET(ret);
  for (const auto& oss : oss_vec) {
    cout << oss.get_id() << ", " << oss.get_key() << ", " << oss.get_value()
         << endl;
  }
  return 0;
}
