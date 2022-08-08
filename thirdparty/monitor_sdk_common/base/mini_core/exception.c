
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

#define _GNU_SOURCE

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <ucontext.h>
#include <unistd.h>

#include "bthelper.h"
#include "exception.h"
#include "mem.h"
#include "vma.h"

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#ifndef TASK_MAX
#define TASK_MAX 64
#endif

#define SIG_DEFAULT_LOG_PATH "/data/coredump/"
#define SIG_STACK_SIZE (16 * 1024)     /* 信号函数栈大小 */
#define SIG_STACK_AGLIN_SIZE (1 << 20) /* 栈基址对齐大小 */
#define SIG_STACK_PROTECT_SIZE \
  (SIG_STACK_AGLIN_SIZE * 4)             /* 信号函数栈保护区域大小 */
#define SIG_HEAP_SIZE (16 * 1024 * 1024) /* 用于信号处理函数使用的堆空间 */
#define PTR_AGLIN(_ptr, _aglin) \
  (void *)((unsigned long)(_ptr) / SIG_STACK_AGLIN_SIZE * SIG_STACK_AGLIN_SIZE)

typedef struct _tag_signal_cb_des_t {
  signal_cb cb;
  void *args;
} signal_cb_des_t;

static s_heap_t *g_pheap = NULL;
static pthread_spinlock_t lock;
static signal_cb_des_t g_cb_des;

/**
 * @brief 获取线程PID
 */
pid_t syscall_gettid(void) { return syscall(SYS_gettid); }

/**
 * @brief 重置信号处理函数
 */
void reset_signal_handler(void) {
  signal(SIGSEGV, SIG_DFL);
  signal(SIGBUS, SIG_DFL);
  signal(SIGFPE, SIG_DFL);
  signal(SIGILL, SIG_DFL);
  signal(SIGABRT, SIG_DFL);
  signal(SIGTERM, SIG_DFL);
}

/**
 * @brief 调用注册的信号回调函数
 */
void call_register_cb(int signo) {
  if (g_cb_des.cb) g_cb_des.cb(signo, g_cb_des.args);
}

void getNameByPid(pid_t pid, char *task_name) {
  char proc_pid_path[TASK_MAX];
  char buf[PATH_MAX];

  sprintf(proc_pid_path, "/proc/%d/status", pid);
  FILE *fp = fopen(proc_pid_path, "r");
  if (NULL != fp) {
    if (fgets(buf, PATH_MAX - 1, fp) == NULL) {
      fclose(fp);
    } else {
      fclose(fp);
      sscanf(buf, "%*s %s", task_name);
    }
  }
}

/**
 * @brief 创建LOG目录
 */
int create_log_dir(time_t now) {
  int ret;
  char path[PATH_MAX];
  pid_t pid = getpid();
  char task_name[TASK_MAX];

  getNameByPid(pid, task_name);

  snprintf(path, sizeof(path), SIG_DEFAULT_LOG_PATH "%s-%d-%d-%llu", task_name,
           pid, syscall_gettid(), (unsigned long long)now);

  ret = mkdir(path, 0777);
  if (ret == -1) {
    printf("mkdir fail.\n");
    return -1;
  }

  return 0;
}

/**
 * @brief 打开backtrace.dat文件，没有就创建
 */
int open_backtrace_file(time_t now) {
  int fd;
  char path[PATH_MAX];
  pid_t pid = getpid();
  char task_name[TASK_MAX];

  getNameByPid(pid, task_name);

  snprintf(path, sizeof(path),
           SIG_DEFAULT_LOG_PATH "%s-%d-%d-%llu/backtrace.dat", task_name, pid,
           syscall_gettid(), (unsigned long long)now);

  fd = open(path, O_RDWR | O_CREAT, 0666);
  if (fd == -1) {
    return -1;
  }

  return fd;
}

int open_stkdata_file(time_t now) {
  int fd;
  char path[PATH_MAX];
  pid_t pid = getpid();
  char task_name[TASK_MAX];

  getNameByPid(pid, task_name);

  snprintf(path, sizeof(path), SIG_DEFAULT_LOG_PATH "%s-%d-%d-%llu/stack.dat",
           task_name, pid, syscall_gettid(), (unsigned long long)now);

  fd = open(path, O_RDWR | O_CREAT, 0666);
  if (fd == -1) {
    return -1;
  }

  return fd;
}

/**
 * @brief 打开maps.dat文件，没有就创建
 */
int open_maps_file(time_t now) {
  int fd;
  char path[PATH_MAX];
  pid_t pid = getpid();
  char task_name[TASK_MAX];

  getNameByPid(pid, task_name);

  snprintf(path, sizeof(path), SIG_DEFAULT_LOG_PATH "%s-%d-%d-%llu/maps.dat",
           task_name, pid, syscall_gettid(), (unsigned long long)now);

  fd = open(path, O_RDWR | O_CREAT, 0666);
  if (fd == -1) {
    return -1;
  }

  return fd;
}

/**
 * @brief 打开signal_info.dat文件，没有就创建
 */
int open_signal_info_file(time_t now) {
  int fd;
  char path[PATH_MAX];
  pid_t pid = getpid();
  char task_name[TASK_MAX];

  getNameByPid(pid, task_name);

  snprintf(path, sizeof(path),
           SIG_DEFAULT_LOG_PATH "%s-%d-%d-%llu/signal_info.dat", task_name, pid,
           syscall_gettid(), (unsigned long long)now);

  fd = open(path, O_RDWR | O_CREAT, 0666);
  if (fd == -1) {
    return -1;
  }

  return fd;
}

/**
 * @brief 保存/proc/self/maps
 */
int save_maps(time_t now, vma_list_t *vma_list) {
  int ret;
  int fd;

  fd = open_maps_file(now);
  if (fd < 0) {
    return -1;
  }

  ret = show_vma_list_fd(fd, vma_list);

  close(fd);

  return ret;
}

/**
 * @brief 读取启动命令行
 */
int read_comm(char *buf, int len) {
  int fd;
  int rlen;
  int loop;

  fd = open("/proc/self/cmdline", O_RDONLY);
  if (fd == -1) {
    return -1;
  }

  rlen = read_fd(fd, buf, len);
  if (rlen < 0) {
    close(fd);
    return -2;
  }

  loop = 0;
  while (loop < rlen) {
    if (buf[loop] == '\0') {
      buf[loop] = ' ';
    }

    loop++;
  }

  buf[rlen - 1] = '\0';
  close(fd);
  return rlen;
}

/**
 * @brief 打印异常信号详细信息
 */
int show_signal_info_fd(int fd, siginfo_t *signal_info, ucontext_t *ucontext,
                        s_heap_t *heap) {
  int ret;
  int blen, len = 0;
  char *buf;
  char comm[256];

  if (NULL == signal_info || NULL == ucontext || NULL == heap) {
    return -1;
  }

  // 1. 分配空间用于组装字符串
  blen = 10240;
  buf = s_heap_alloc(heap, blen);
  if (NULL == buf) {
    return -2;
  }

  // 2. 读取进程启动命令行
  ret = read_comm(comm, sizeof(comm));
  if (ret < 0) {
    return -3;
  }

  // 3. 打印信号上下文信息
  len += snprintf(buf + len, blen - len, "[ %lu ] Pid: %d, Tid: %d, comm: %s\n",
                  time(NULL), getpid(), syscall_gettid(), comm);

  if (signal_info->si_code <= 0) {
    len += snprintf(buf + len, blen - len,
                    "Signal: %d errno: %d code: %d was generated by a process, "
                    "Pid: %d, Uid: %d\n",
                    signal_info->si_signo, signal_info->si_errno,
                    signal_info->si_code, signal_info->si_pid,
                    signal_info->si_uid);
  } else {
    len += snprintf(buf + len, blen - len,
                    "Signal: %d errno: %d code: %d addr_ref: 0x%lx\n",
                    signal_info->si_signo, signal_info->si_errno,
                    signal_info->si_code, (long)signal_info->si_addr);
  }

  len += snprintf(buf + len, blen - len, "Registers info:\n");

#if defined(__amd64__) || defined(__x86_64__)
  len += snprintf(
      buf + len, blen - len,
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n"
      "%-16s0x%-16llx\n",
      "rax", (unsigned long long)ucontext->uc_mcontext.gregs[REG_RAX], "rbx",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_RBX], "rcx",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_RCX], "rdx",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_RDX], "rsi",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_RSI], "rdi",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_RDI], "rbp",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_RBP], "rsp",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_RSP], "r8",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_R8], "r9",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_R9], "r10",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_R10], "r11",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_R11], "r12",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_R12], "r13",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_R13], "r14",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_R14], "r15",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_R15], "rip",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_RIP], "eflags",
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_EFL]);
#elif defined(__i386__)
  len += snprintf(buf + len, blen - len,
                  "%-16s0x%-16x\n"
                  "%-16s0x%-16x\n"
                  "%-16s0x%-16x\n"
                  "%-16s0x%-16x\n"
                  "%-16s0x%-16x\n"
                  "%-16s0x%-16x\n"
                  "%-16s0x%-16x\n"
                  "%-16s0x%-16x\n"
                  "%-16s0x%-16x\n"
                  "%-16s0x%-16x\n",
                  "eax", ucontext->uc_mcontext.gregs[REG_EAX], "ecx",
                  ucontext->uc_mcontext.gregs[REG_ECX], "edx",
                  ucontext->uc_mcontext.gregs[REG_EDX], "ebx",
                  ucontext->uc_mcontext.gregs[REG_EBX], "esp",
                  ucontext->uc_mcontext.gregs[REG_ESP], "ebp",
                  ucontext->uc_mcontext.gregs[REG_EBP], "esi",
                  ucontext->uc_mcontext.gregs[REG_ESI], "edi",
                  ucontext->uc_mcontext.gregs[REG_EDI], "eip",
                  ucontext->uc_mcontext.gregs[REG_EIP], "eflags",
                  ucontext->uc_mcontext.gregs[REG_EFL]);
#else
#error "Linux cpu arch not supported"
#endif

  write_all(fd, buf, len);

  return 0;
}

int show_stkdata_fd(int fd, ucontext_t *ucontext, s_heap_t *heap,
                    vma_list_t *vma_list) {
  char *buf;
  int blen, len = 0;
  unsigned long long start, end;

#if defined(__amd64__) || defined(__x86_64__)
  unsigned long long sp =
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_RSP];
  unsigned long long bp =
      (unsigned long long)ucontext->uc_mcontext.gregs[REG_RBP];
  unsigned long long ptr;
  int scan = 8;
#elif defined(__i386__)
  unsigned int sp = (unsigned int)ucontext->uc_mcontext.gregs[REG_ESP];
  unsigned int bp = (unsigned int)ucontext->uc_mcontext.gregs[REG_EBP];
  unsigned int ptr;
  int scan = 4;
#else
#error "Linux cpu arch not supported"
#endif

  blen = 10240;
  buf = s_heap_alloc(heap, blen);
  if (NULL == buf) {
    return -2;
  }

  len += snprintf(buf + len, blen - len,
                  "[ Current frame ] ESP: 0x%-16llx, EBP: 0x%-16llx\n", sp, bp);

  start = sp;
  end = start + 2048;
  for (ptr = start; ptr < end; ptr = ptr + scan) {
    if (!vma_check_prot(vma_list, (void *)ptr, sizeof(ptr), VMA_PROT_READ))
      break;

#if defined(__amd64__) || defined(__x86_64__)
    len += snprintf(buf + len, blen - len, "0x%016llx %016llx\n", ptr,
                    *(unsigned long long *)ptr);
#elif defined(__i386__)
    len += snprintf(buf + len, blen - len, "0x%08x %08x\n", ptr,
                    *(unsigned int *)ptr);
#else
#error "Linux cpu arch not supported"
#endif
  }

  write_all(fd, buf, len);

  return 0;
}

/**
 * @brief 保存信号的上下文
 */
int save_signal_info(time_t now, siginfo_t *signal_info, ucontext_t *ucontext,
                     s_heap_t *heap) {
  int ret;
  int fd;

  fd = open_signal_info_file(now);
  if (fd < 0) {
    return -1;
  }

  ret = show_signal_info_fd(fd, signal_info, ucontext, heap);

  close(fd);

  return ret;
}

/**
 * @brief 保存调用栈日志
 */
int save_backtrace(time_t now, ucontext_t *ucontext, s_heap_t *heap,
                   vma_list_t *vma_list) {
  int ret;
  int fd;

  fd = open_backtrace_file(now);
  if (fd < 0) {
    return -1;
  }

  ret = show_backtrace_fd(fd, ucontext, heap, vma_list);

  close(fd);

  return ret;
}

int save_stack(time_t now, ucontext_t *ucontext, s_heap_t *heap,
               vma_list_t *vma_list) {
  int ret;
  int fd;

  fd = open_stkdata_file(now);
  if (fd < 0) {
    return -1;
  }

  ret = show_stkdata_fd(fd, ucontext, heap, vma_list);

  close(fd);

  return ret;
}

/**
 * @brief 信号处理主函数
 * @info
 */
void signal_handler(int signal_no, siginfo_t *signal_info, void *ucontext) {
  int ret, errno_bak;
  time_t now;
  s_heap_t *heap;
  vma_list_t *vma_list;

  // 多个线程同时产生core，会同时执行handle，所以需要互斥
  pthread_spin_lock(&lock);

  // 备份errno，因为信号处理函数可能会改变之前上下文的errno
  errno_bak = errno;

  if (NULL == signal_info || NULL == ucontext || 0 == signal_no) {
    goto RET;
  }

  /**
   * 先执行安全的操作，不会导致再core
   */

  // 1. 初始化heap,后续使用
  heap = g_pheap;
  if (NULL == heap) {
    goto RET;
  }

  // 2. 读取vma信息
  vma_list = vma_list_create(heap);
  if (NULL == vma_list) {
    goto RET;
  }

  // 3. 创建目录
  now = time(NULL);
  ret = create_log_dir(now);
  if (ret < 0) {
    goto RET;
  }

  // 4. 保存vma信息
  save_maps(now, vma_list);

  // 5. 保存信号上下文信息
  save_signal_info(now, signal_info, (ucontext_t *)ucontext, heap);

  // 6. 先重置信号处理函数，后面的操作可能导致再次coredump
  reset_signal_handler();

  /**
   * @brief 下面的信息可能导致再次core
   */

  // 7. 保存调用栈信息
  save_backtrace(now, (ucontext_t *)ucontext, heap, vma_list);

  // 8. 保存栈上数据
  save_stack(now, (ucontext_t *)ucontext, heap, vma_list);

  // 9. 释放堆空间
  s_heap_destory(heap);
  g_pheap = NULL;

  // 10. 调用注册的通知函数
  call_register_cb(signal_no);

  // 11. 恢复errno
  errno = errno_bak;

  pthread_spin_unlock(&lock);

  return;

RET:
  errno = errno_bak;
  pthread_spin_unlock(&lock);
  reset_signal_handler();
  if (g_pheap) {
    s_heap_destory(g_pheap);
    g_pheap = NULL;
  }
  return;
}

/**
 * @brief 替换信号处理函数的栈
 * @info  虚拟地址格式
 * @      | protect | stack | rdonly | protect |
 */
int signal_stack_init() {
  int prot = PROT_READ | PROT_WRITE;
  int flags = MAP_PRIVATE | MAP_ANON;
  int memsize;
  void *mem;
  char *ss_start;
  char *ss_end;
  char *p1_start;
  char *p1_end;
  char *p2_start;
  char *p2_end;
  stack_t stack;

  /* 申请栈虚拟内存 */
  memsize = SIG_STACK_PROTECT_SIZE + SIG_STACK_SIZE;
  mem = mmap(NULL, memsize, prot, flags, -1, 0);
  if (mem == (void *)MAP_FAILED) {
    return -1;
  }

  /* 计算各个区间的范围 */
  p1_start = (char *)mem;
  p1_end = (char *)PTR_AGLIN(p1_start + SIG_STACK_PROTECT_SIZE / 2,
                             SIG_STACK_AGLIN_SIZE);
  ss_start = p1_end;
  ss_end = ss_start + SIG_STACK_SIZE;
  p2_start = ss_end;
  p2_end = (char *)mem + memsize;

  /* 设置栈保护区域 */
  mprotect(p1_start, p1_end - p1_start, PROT_NONE);
  mprotect(p2_start, p2_end - p2_start, PROT_NONE);

  /* 替换信号处理函数的栈 */
  stack.ss_sp = (void *)ss_start;
  stack.ss_size = SIG_STACK_SIZE;
  stack.ss_flags = 0;
  sigaltstack(&stack, (stack_t *)0);

  return 0;
}

/**
 * @brief 安装信号处理函数
 */
int install_signal_handler(signal_cb cb, void *args) {
  int ret;
  struct sigaction act;

  g_cb_des.cb = cb;
  g_cb_des.args = args;

  // 1. 初始化信号处理函数的栈
  ret = signal_stack_init();
  if (ret) {
    printf("[ERROR] change signal handler stack failed, ret [%d]\n", ret);
    return -1;
  }

  // 2。初始化自旋锁，作为线程安全考虑
  pthread_spin_init(&lock, 0);

  // 3. 申请handle独用的堆，因为handle中不能使用malloc，是非异步信号安全的
  g_pheap = s_heap_init(SIG_HEAP_SIZE);
  if (!g_pheap) {
    printf("[ERROR] malloc signal heap failed, ret [%p]\n", g_pheap);
    return -1;
  }

  memset(&act, 0, sizeof(act));
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO | SA_ONSTACK;
  act.sa_sigaction = &signal_handler;

  /* 注册信号处理函数 */
  sigaction(SIGSEGV, &act, NULL);
  sigaction(SIGBUS, &act, NULL);
  sigaction(SIGFPE, &act, NULL);
  sigaction(SIGILL, &act, NULL);
  sigaction(SIGABRT, &act, NULL);
  sigaction(SIGTERM, &act, NULL);

  return 0;
}
