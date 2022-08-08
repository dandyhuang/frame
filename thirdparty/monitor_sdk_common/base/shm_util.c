// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: libin <langman.bin@gmail.com>
#include "thirdparty/monitor_sdk_common/base/shm_util.h"
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/mman.h>

/* shm layout: |1page mprotect|page align data|1page mprotect| 
 * mprotect avoid mem Cross-border 
 */
#define PAGE_ALIGN(size, align)    ((((size) + ((align) - 1)) / (align)) * (align))

static void s_shminit(struct ShmUtil* shmu, uint32_t key, uint64_t sz) {
  shmu->key_ = key;
  // row to page_size align
  shmu->page_size_ = getpagesize();  
  shmu->size_ = PAGE_ALIGN(sz, shmu->page_size_);
  /* head and tail mprotect zone are all one page */
  shmu->tot_size_ = shmu->size_ + 2 * shmu->page_size_;
}

// return 0 for success(not created), 1 for success(created), -1 for fail
static int s_shmcreate(struct ShmUtil* shmu) {
  if (shmu == NULL) return -1;
  int shmid = shmget(shmu->key_, shmu->tot_size_, 0666);
  int created = 0;
  if (shmid < 0) {
    shmid = shmget(shmu->key_, shmu->tot_size_, 0666 | IPC_CREAT);
    if (shmid < 0) return -2;
    created = 1;
  }
  void* p = shmat(shmid, NULL, 0);
  if (p == (void*)-1) return -3;
  /* shm base addr  */
  shmu->base_ = p;
  /* data = base + HEAD_PROTECT_SIZE(one page) */
  shmu->data_ = (char *)(shmu->base_) + shmu->page_size_;
  if (created) {
    memset(shmu->data_, 0, shmu->size_);
  }
  /* mprotect for head and tail zone*/
  char *head_pstart = (char *)(shmu->base_);
  char *tail_pstart = (char *)(shmu->data_) + shmu->size_;
  if (mprotect(head_pstart, shmu->page_size_, PROT_NONE)) {
    shmutil_destroy(shmu);
    return -4;
  }
  if (mprotect(tail_pstart, shmu->page_size_, PROT_NONE)) {
    shmutil_destroy(shmu);
    return -5;
  }

  return created;
}

// return 0 for success,  <0 for failure
static int s_shmget(struct ShmUtil* shmu) {
  if (shmu == NULL) return -1;
  // 1. get real size
  int shmid = shmget(shmu->key_, 0, 0666);
  if (shmid < 0) return -2;
  struct shmid_ds ds;
  if (shmctl(shmid, IPC_STAT, &ds) != 0) return -3;
  shmu->page_size_ = getpagesize();
  shmu->tot_size_ = ds.shm_segsz;
  /* shm include data zone, head and tail mprotect zone(one page) */
  shmu->size_ = shmu->tot_size_ - 2 * shmu->page_size_;
  // 2. attach
  void* p = shmat(shmid, NULL, 0);
  if (p == (void*)(-1)) return -4;
  shmu->base_ = p;
  shmu->data_ = (char *)p + shmu->page_size_;
  return 0;
}

static void* s_getdata(const struct ShmUtil* shmu) {
  return shmu != NULL ? shmu->data_ : NULL;
}

static uint64_t s_getsize(const struct ShmUtil* shmu) {
  return shmu != NULL ? shmu->size_ : (uint64_t)(-1);
}

// return 0 for success,  <0 for failure
static int s_shmdetatch(struct ShmUtil* shmu) {
  if (shmu != NULL && shmu->base_) {
    shmdt(shmu->base_);
    shmu->base_ = NULL;
    shmu->data_ = NULL;
    return 0;
  }
  return -1;
}

// return 0 for success,  <0 for failure
static int s_shmdelete(struct ShmUtil* shmu) {
  if (shmu == NULL) return -1;
  int shmid = shmget(shmu->key_, 0, 0);
  s_shmdetatch(shmu);
  if (shmid >= 0 && 0 != shmctl(shmid, IPC_RMID, NULL)) return -2;
  return 0;
}

// return 0 for success, <0 for error
int shmutil_init(struct ShmUtil* shmu) {
  if (shmu == NULL) return -1;
  shmu->key_ = 0;
  shmu->page_size_ = 0;
  shmu->tot_size_ = 1;
  shmu->size_ = 1;
  shmu->base_ = NULL;
  shmu->data_ = NULL;
  shmu->init = s_shminit;
  shmu->create_shm = s_shmcreate;
  shmu->get_shm = s_shmget;
  shmu->get_data = s_getdata;
  shmu->get_size = s_getsize;
  shmu->detach_shm = s_shmdetatch;
  shmu->delete_shm = s_shmdelete;
  return 0;
}

// return 0 for success, <0 for error
int shmutil_destroy(struct ShmUtil* shmu) { return s_shmdetatch(shmu); }

