// // Copyright (c) 2013, The Toft Authors.
// // All rights reserved.
// //
// // Author: Cao shuzhen
//

#ifndef BASE_MINI_CORE_BTHELPER_H_
#define BASE_MINI_CORE_BTHELPER_H_

#include <ucontext.h>
#include "thirdparty/monitor_sdk_common/base/mini_core/mem.h"
#include "thirdparty/monitor_sdk_common/base/mini_core/vma.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/**
 * @brief 输出调用栈信息到对应描述符
 * @info  使用libunwind\libbacktrace库
 */
int show_backtrace_fd(int fd, ucontext_t *ucontext, s_heap_t *heap,
                      vma_list_t *vma_list);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif  //  BASE_MINI_CORE_BTHELPER_H_
