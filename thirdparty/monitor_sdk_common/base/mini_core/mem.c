
/**
 * Tencent is pleased to support the open source community by making MSEC
 * available.
 *
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the GNU General Public License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may
 * obtain a copy of the License at
 *
 *     https://opensource.org/licenses/GPL-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#include <errno.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "mem.h"

#define M_UNIT (1024 * 1024)
#define ALIGN_SIZE (16)

/**
 * @brief 初始化自定义heap
 */
s_heap_t *s_heap_init(int size) {
  void *addr;
  int prot = PROT_READ | PROT_WRITE;
  int flags = MAP_PRIVATE | MAP_ANON;
  s_heap_t *heap;

  if (size <= 0) {
    return NULL;
  }

  size = size / M_UNIT * M_UNIT;

  addr = mmap(NULL, size, prot, flags, -1, 0);
  if (addr == (void *)MAP_FAILED) {
    return NULL;
  }

  heap = (s_heap_t *)addr;
  heap->addr = addr;
  heap->used = sizeof(s_heap_t);
  heap->size = size;

  return heap;
}

/**
 * @brief 释放自定义heap
 */
void s_heap_destory(s_heap_t *heap) { munmap(heap->addr, heap->size); }

/**
 * @brief 从自定义heap中分配空间
 */
void *s_heap_alloc(s_heap_t *heap, int size) {
  void *addr;

  if (size <= 0 || NULL == heap) {
    return NULL;
  }

  size = (size + ALIGN_SIZE - 1) / ALIGN_SIZE * ALIGN_SIZE;
  if (heap->used + size > heap->size) {
    return NULL;
  }

  addr = (void *)((char *)heap->addr + heap->used);
  heap->used += size;

  return addr;
}

/**
 *  * @brief 输出buf到文件描述符
 *   */
int write_all(int fd, void *mem, int len) {
  char *buf = (char *)mem;
  int ret;
  int wlen = 0;

  while (wlen != len) {
    ret = write(fd, buf + wlen, len - wlen);
    if (ret == -1) {
      if (errno == EAGAIN || errno == EINTR) {
        continue;
      }
      return -2;
    }

    wlen += ret;
  }

  return 0;
}

/**
 *  * @brief 从文件描述符读取数据到buf
 *   */
int read_fd(int fd, void *mem, int len) {
  char *buf = (char *)mem;
  int ret;
  int rlen = 0;

  while (rlen != len) {
    ret = read(fd, buf + rlen, len - rlen);
    if (ret == -1) {
      if (errno == EAGAIN || errno == EINTR) {
        continue;
      }
      return -2;
    }

    if (ret == 0) {
      break;
    }

    rlen += ret;
  }

  return rlen;
}
