
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

#if defined(__amd64__) || defined(__x86_64__)
#include "thirdparty/libunwind/libunwind-x86_64.h"
#elif defined(__i386__)
#include "libunwind-x86.h"
#else
#error "Linux cpu arch not supported"
#endif

#include <stdio.h>
#include <string.h>
#include "mem.h"
#include "vma.h"

/**
 * @brief 输出调用栈信息到对应描述符
 * @info  使用libunwind\libbacktrace库
 */
int show_backtrace_fd(int fd, ucontext_t *ucontext, s_heap_t *heap,
                      vma_list_t *vma_list) {
  int ret, len;
  char func[128];
  char wbuf[128];
  unw_word_t ip;
  unw_word_t off;
  unw_cursor_t cursor;

  ret = unw_init_local(&cursor, ucontext);
  if (ret < 0) {
    return -1;
  }

  do {
    unw_get_reg(&cursor, UNW_REG_IP, &ip);
    ret = unw_get_proc_name(&cursor, func, sizeof(func), &off);
    if (ret < 0) {
      return -2;
    }

    len = snprintf(wbuf, sizeof(wbuf), "  0x%016lx (%s+%lu) \n",
                   (unsigned long)ip, func, (unsigned long)off);
    write_all(fd, wbuf, len);
  } while ((ret = unw_step(&cursor)) > 0);

  return 0;
}
