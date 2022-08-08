// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Cao shuzhen
// //

#ifndef BASE_MINI_CORE_EXCEPTION_H_
#define BASE_MINI_CORE_EXCEPTION_H_

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/**
 * @brief 信号处理回调函数
 */
typedef void (*signal_cb)(int, void *);

/**
 * @brief 重置信号处理函数
 */
void reset_signal_handler(void);

/**
 * @brief 安装信号处理函数
 */
int install_signal_handler(signal_cb cb, void *args);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif  //  BASE_MINI_CORE_EXCEPTION_H_
