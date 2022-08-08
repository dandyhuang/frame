// // Copyright (c) 2013, The Toft Authors.
// // // All rights reserved.
// // //
// // // Author: Cao shuzhen
// //
//

#ifndef BASE_MINI_CORE_MEM_H_
#define BASE_MINI_CORE_MEM_H_

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/**
 * @brief 自定义heap管理数据结构
 * @info  由于在信号处理函数中不能使用malloc等函数，直接自建一个heap
 *        分配的内存不需要释放，信号处理函数执行完后统一释放
 */
typedef struct _tag_s_heap {
  void *addr;
  int used;
  int size;
} s_heap_t;

/**
 *  * @brief 输出buf到文件描述符
 *   */
int write_all(int fd, void *mem, int len);

/**
 *  * @brief 从文件描述符读取数据到buf
 *   */
int read_fd(int fd, void *mem, int len);

/**
 * @brief 初始化自定义heap
 */
s_heap_t *s_heap_init(int size);

/**
 * @brief 从自定义heap中分配空间
 */
void *s_heap_alloc(s_heap_t *heap, int size);

/**
 * @brief 释放自定义heap
 */
void s_heap_destory(s_heap_t *heap);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif  // BASE_MINI_CORE_MEM_H_
