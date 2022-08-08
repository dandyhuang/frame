// // Copyright (c) 2013, The Toft Authors.
// // // All rights reserved.
// // //
// // // Author: Cao shuzhen
// //
//

#ifndef BASE_MINI_CORE_VMA_H_
#define BASE_MINI_CORE_VMA_H_

#include <stdint.h>
#include "thirdparty/monitor_sdk_common/base/mini_core/mem.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/**
 * 虚拟内存标志位宏定义
 */
#define VMA_PROT_READ 0x00000001
#define VMA_PROT_WRITE 0x00000002
#define VMA_PROT_EXEC 0x00000004
#define VMA_PROT_SHARED 0x00000008
#define VMA_PROT_PRIVATE 0x00000010

/**
 * 虚拟内存描述数据结构
 */
typedef struct _tag_vma {
  void *next;
  void *start;
  void *end;
  void *file;
  void *dev;
  int flags;
  uint64_t ino;
  uint64_t offset;
} vma_t;

/**
 * 虚拟内存链表数据结构
 */
typedef struct _tag_vma_list {
  vma_t *head;
  vma_t *tail;
  int size;
} vma_list_t;

/**
 * @brief 检查[addr, addr+len)是否有相应的权限
 * @return  =1 有相应权限
 *          =0 没有权限
 */
int vma_check_prot(vma_list_t *list, void *addr, int len, int prot);

/**
 * @brief 返回地址所在的二进制文件
 */
const char *vma_get_bin_path(vma_list_t *list, void *addr);

/**
 * @brief 获取当前进程的虚拟地址空间信息
 * @info  为保证信号处理函数中能够安全调用，需要传入自定义的heap
 */
vma_list_t *vma_list_create(s_heap_t *heap);

/**
 * @brief 将虚拟地址信息输出到fd 同("/proc/self/maps")
 */
int show_vma_list_fd(int fd, vma_list_t *list);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif  //  BASE_MINI_CORE_VMA_H_
