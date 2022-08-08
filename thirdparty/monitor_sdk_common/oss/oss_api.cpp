// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: libin <langman.bin@gmail.com>
#include "thirdparty/monitor_sdk_common/oss/oss_api.h"
#include <sys/file.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include "thirdparty/monitor_sdk_common/base/shm_util.h"
#include "thirdparty/monitor_sdk_common/oss/oss_agent_api.h"

#define OSS_SHM_ID 0x3eeff00
#define OSS_MAP_COUNT 2
#define OSS_ATTR_ENTRY_COUNT (1024 * 128)
// for test
// #define OSS_ATTR_ENTRY_COUNT (128)
#define OSS_ONE_ATTR_SIZE 128

#define MAGIC_NUM1 0x650a218
#define MAGIC_NUM2 0x138a4f2

#define CREATE_SHM_LOCK "/var/run/.oss_shm_lock"

enum OP_TYPE { OP_INC = 0, OP_SET, OP_MAX, OP_MIN };

typedef struct AttrValue {
  uint32_t value;
} __attribute__((__packed__)) AttrValue;

typedef struct OneAttr {
  AttrValue val_arr[OSS_ONE_ATTR_SIZE];
} __attribute__((__packed__)) OneAttr;

typedef struct OssMap {
  OneAttr attr_arr[OSS_ATTR_ENTRY_COUNT];
} __attribute__((__packed__)) OssMap;

typedef struct OssAttr {
  uint32_t magic1;
  uint32_t idx;  // modified by agent
  OssMap id_map[OSS_MAP_COUNT];
  uint32_t reserved[64];
  uint32_t magic2;
} OssAttr;

static OssAttr* g_oss_attr = NULL;

static inline bool CheckInput(uint32_t ossid, uint32_t key) {
  return ossid < OSS_ATTR_ENTRY_COUNT && key < OSS_ONE_ATTR_SIZE;
}

static inline bool CheckMagic() {
  return g_oss_attr && g_oss_attr->magic1 == MAGIC_NUM1 &&
         g_oss_attr->magic2 == MAGIC_NUM2;
}

/* if return 0, just get shm;
 * if return < 0, lock fail;
 * if return > 0, lock success, return lock_fd */
static inline int CheckLockShm(struct ShmUtil* shm_util) {
  // shm already created, just get it.
  if (!(shm_util->get_shm(shm_util))) return 0;
  int lock_fd = open(CREATE_SHM_LOCK, O_RDONLY | O_CREAT, 0640);
  if (lock_fd < 0) return -1;
  int ret = flock(lock_fd, LOCK_EX);
  if (ret < 0) {
    close(lock_fd);
    return -2;
  }
  return lock_fd;
}

static bool GetShm() {
  if (g_oss_attr != NULL) return true;
  struct ShmUtil s_shmu;
  shmutil_init(&s_shmu);
  s_shmu.init(&s_shmu, OSS_SHM_ID, sizeof(OssAttr));
  // avoid several process create shm at the same time.
  int lockfd = CheckLockShm(&s_shmu);
  // if lock fail, return false.
  if (lockfd < 0) return false;
  int created = s_shmu.create_shm(&s_shmu);
  g_oss_attr = reinterpret_cast<OssAttr*>(s_shmu.get_data(&s_shmu));
  if (created == 1) {
    g_oss_attr->magic1 = MAGIC_NUM1;
    g_oss_attr->magic2 = MAGIC_NUM2;
  }
  // if lockfd == 0, just get shm, no need to lock
  if (lockfd > 0) {
    flock(lockfd, LOCK_UN);
    close(lockfd);
  }
  return true;
}

static void atomic_op(OP_TYPE optype, volatile uint32_t* data, uint32_t value) {
  if (optype == OP_INC) {
    __sync_fetch_and_add(data, value);
  } else if (optype == OP_SET) {
    __sync_lock_test_and_set(data, value);
  } else if (optype == OP_MAX) {
    for (int i = 0; i < 10; ++i) {
      if (*data < value) {
        uint32_t oldv = __sync_lock_test_and_set(data, value);
        if (oldv > value) value = oldv;  // have been set by other threads
      } else {
        break;
      }
    }
  } else if (optype == OP_MIN) {
    for (int i = 0; i < 10; ++i) {
      if (*data > value) {
        uint32_t oldv = __sync_lock_test_and_set(data, value);
        if (oldv < value) value = oldv;  // have been set by other threads
      } else {
        break;
      }
    }
  }
}

static int OssAttrImpl(uint32_t ossid, uint32_t key, uint32_t val,
                       OP_TYPE optype) {
  if (!CheckInput(ossid, key)) return -1;
  if (!GetShm()) return -2;
  if (!CheckMagic()) return -3;
  volatile uint32_t* idx = &(g_oss_attr->idx);
  if (*idx >= OSS_MAP_COUNT) return -4;
  volatile uint32_t* data =
      &(g_oss_attr->id_map[*idx].attr_arr[ossid].val_arr[key].value);
  atomic_op(optype, data, val);
  return 0;
}

int OssAttrInc(uint32_t ossid, uint32_t key, uint32_t val) {
  return OssAttrImpl(ossid, key, val, OP_INC);
}

int OssAttrSet(uint32_t ossid, uint32_t key, uint32_t val) {
  return OssAttrImpl(ossid, key, val, OP_SET);
}

int OssAttrSetMax(uint32_t ossid, uint32_t key, uint32_t val) {
  return OssAttrImpl(ossid, key, val, OP_MAX);
}

int OssAttrSetMin(uint32_t ossid, uint32_t key, uint32_t val) {
  return OssAttrImpl(ossid, key, val, OP_MIN);
}

static uint32_t SwitchShm() {
  volatile uint32_t* idx = &(g_oss_attr->idx);
  uint32_t old_idx = __sync_fetch_and_xor(idx, 0x1);
  // std::cout<< "switched idx, old idx " << old_idx << std::endl;
  return old_idx;
}

template <typename OP>
static void TraverseOssInfo(const OssMap& oss_map, OP op) {
  for (uint32_t i = 0; i < OSS_ATTR_ENTRY_COUNT; ++i) {
    const auto& attr = oss_map.attr_arr[i];
    for (uint32_t j = 0; j < OSS_ONE_ATTR_SIZE; ++j) {
      op(i, j, attr.val_arr[j].value);
    }
  }
}

int SwitchAndGetAttrData(std::vector<OssInfo>& oss_vec) {
  if (!GetShm()) return -1;
  uint32_t old_idx = SwitchShm();
  uint32_t cnt = 0;
  TraverseOssInfo(g_oss_attr->id_map[old_idx],
                  [&](uint32_t ossid, uint32_t key, uint32_t val) {
                    if (val) ++cnt;
                  });

  oss_vec.clear();
  oss_vec.reserve(cnt);

  TraverseOssInfo(g_oss_attr->id_map[old_idx],
                  [&](uint32_t ossid, uint32_t key, uint32_t val) {
                    if (val) {
                      oss_vec.push_back(OssInfo(ossid, key, val));
                    }
                  });
  // reset to zero
  memset(&g_oss_attr->id_map[old_idx], 0x0, sizeof(OssMap));
  return 0;
}
