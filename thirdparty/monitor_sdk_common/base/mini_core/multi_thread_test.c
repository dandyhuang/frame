
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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "exception.h"

pthread_t ntid;

void cb(int signo, void *args) { printf("signal callback!!\n"); }

void c(void) {
  int *p = NULL;
  *p = 0;
}

void b(void) { c(); }

void a(void) { b(); }

void cc(void) {
  int a = 0x1234;
  int *p = NULL;
  printf("a=%d.\n", a);
  *p = 0;
}

void bb(void) {
  int a = 0x3456;
  printf("a=%d.\n", a);
  cc();
}

void aa(void) {
  int a = 0x4567;
  printf("a=%d.\n", a);
  bb();
}

void *thread(void *arg) {
  //    printf("new thread %u.\n", (unsigned int)ntid);

  aa();

  return ((void *)0);
}

int main() {
  int ret;
  ret = install_signal_handler(cb, NULL);
  if (ret < 0) {
    printf("install signal handler failed. ret [%d]\n", ret);
    return -1;
  }

#if 1
  if (pthread_create(&ntid, NULL, thread, NULL) != 0) {
    printf("can't create thread.\n");
    return -1;
  }
#endif

  sleep(1);
  printf("main thread .\n");

  a();

  return ret;
}
