// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: libin <langman.bin@gmail.com>

#ifndef COMMON_OSS_AGENT_API_H_
#define COMMON_OSS_AGENT_API_H_
#pragma once

#include <stdint.h>
#include <vector>
class OssInfo {
 public:
  OssInfo(uint32_t id = 0, uint32_t key = 0, uint32_t val = 0)
      : id_(id), key_(key), val_(val) {}
  uint32_t get_id() const { return id_; }
  uint32_t get_key() const { return key_; }
  uint32_t get_value() const { return val_; }

 private:
  uint32_t id_;
  uint32_t key_;
  uint32_t val_;
};

int SwitchAndGetAttrData(std::vector<OssInfo>& oss_vec);

#endif

