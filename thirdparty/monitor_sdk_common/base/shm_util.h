// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: libin <langman.bin@gmail.com>

#ifndef COMMON_BASE_SHM_UTIL_H_
#define COMMON_BASE_SHM_UTIL_H_

#include <stdint.h>

struct ShmUtil;
typedef struct ShmUtil {
  // only set member, key and size
  void (*init)(struct ShmUtil* shmu, uint32_t key, uint64_t sz);

  // return 0 for success(not create), 1 for success(created), -1 for error
  int (*create_shm)(struct ShmUtil* shmu);

  // return 0 for success, <0 for error
  int (*get_shm)(struct ShmUtil* shmu);

  // return 0 for success, <0 for error
  void* (*get_data)(const struct ShmUtil* shmu);

  // return 0 for success, <0 for error
  uint64_t (*get_size)(const struct ShmUtil* shmu);

  // return 0 for success, <0 for error
  int (*detach_shm)(struct ShmUtil* shmu);

  // return 0 for success, <0 for error
  int (*delete_shm)(struct ShmUtil* shmu);

  // data
  uint32_t key_;
  uint32_t page_size_;
  uint64_t size_;
  uint64_t tot_size_;
  void* data_;
  void* base_;
} ShmUtil;

#ifdef __cplusplus
extern "C" {
#endif
// return 0 for success, <0 for error
int shmutil_init(struct ShmUtil* shmu);

// return 0 for success, <0 for error
int shmutil_destroy(struct ShmUtil* shmu);
#ifdef __cplusplus
}
#endif

#endif   // COMMON_BASE_SHM_UTIL_H_

