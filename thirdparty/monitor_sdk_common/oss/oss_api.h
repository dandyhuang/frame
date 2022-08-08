// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: libin <langman.bin@gmail.com>
#ifndef COMMON_OSS_API_H_
#define COMMON_OSS_API_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int OssAttrInc(uint32_t ossid, uint32_t key, uint32_t val);

int OssAttrSet(uint32_t ossid, uint32_t key, uint32_t val);

int OssAttrSetMax(uint32_t ossid, uint32_t key, uint32_t val);

int OssAttrSetMin(uint32_t ossid, uint32_t key, uint32_t val);

#ifdef __cplusplus
}
#endif

#endif  // COMMON_OSS_API_H_

