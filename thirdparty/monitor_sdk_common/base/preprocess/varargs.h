// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2014-08-13

#ifndef COMMON_BASE_PREPROCESS_VARARGS_H
#define COMMON_BASE_PREPROCESS_VARARGS_H
#pragma once

#include "thirdparty/monitor_sdk_common/base/preprocess/stringize.h"

// Count the number of va args
#define COMMON_PP_N_ARGS(...) \
    COMMON_PP_N_ARGS_HELPER1(,##__VA_ARGS__, \
        15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define COMMON_PP_N_ARGS_HELPER1(...) COMMON_PP_N_ARGS_HELPER2(__VA_ARGS__)
#define COMMON_PP_N_ARGS_HELPER2(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, \
                               x11, x12, x13, x14, x15, n, ...) n


// Apply unary macro m to each arg of va
#define COMMON_PP_FOR_EACH_ARGS(m, ...) \
    COMMON_PP_FOR_EACH_ARGS_(COMMON_PP_N_ARGS(__VA_ARGS__), m, ##__VA_ARGS__)

#define COMMON_PP_FOR_EACH_ARGS_(n, m, ...) \
    COMMON_PP_JOIN(COMMON_PP_FOR_EACH_ARGS_, n)(m, ##__VA_ARGS__)

#define COMMON_PP_FOR_EACH_ARGS_0(m)
#define COMMON_PP_FOR_EACH_ARGS_1(m, a1) m(a1)
#define COMMON_PP_FOR_EACH_ARGS_2(m, a1, a2) m(a1) m(a2)
#define COMMON_PP_FOR_EACH_ARGS_3(m, a1, a2, a3) m(a1) m(a2) m(a3)
#define COMMON_PP_FOR_EACH_ARGS_4(m, a1, a2, a3, a4) m(a1) m(a2) m(a3) m(a4)
#define COMMON_PP_FOR_EACH_ARGS_5(m, a1, a2, a3, a4, a5) \
    m(a1) m(a2) m(a3) m(a4) m(a5)
#define COMMON_PP_FOR_EACH_ARGS_6(m, a1, a2, a3, a4, a5, a6) \
    m(a1) m(a2) m(a3) m(a4) m(a5) m(a6)
#define COMMON_PP_FOR_EACH_ARGS_7(m, a1, a2, a3, a4, a5, a6, a7) \
    m(a1) m(a2) m(a3) m(a4) m(a5) m(a6) m(a7)
#define COMMON_PP_FOR_EACH_ARGS_8(m, a1, a2, a3, a4, a5, a6, a7, a8) \
    m(a1) m(a2) m(a3) m(a4) m(a5) m(a6) m(a7) m(a8)
#define COMMON_PP_FOR_EACH_ARGS_9(m, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
    m(a1) m(a2) m(a3) m(a4) m(a5) m(a6) m(a7) m(a8) m(a9)
#define COMMON_PP_FOR_EACH_ARGS_10(m, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) \
    m(a1) m(a2) m(a3) m(a4) m(a5) m(a6) m(a7) m(a8) m(a9) m(a10)
#define COMMON_PP_FOR_EACH_ARGS_11(m, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) \
    m(a1) m(a2) m(a3) m(a4) m(a5) m(a6) m(a7) m(a8) m(a9) m(a10) m(a11)
#define COMMON_PP_FOR_EACH_ARGS_12(m, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) \
    m(a1) m(a2) m(a3) m(a4) m(a5) m(a6) m(a7) m(a8) m(a9) m(a10) m(a11) m(a12)
#define COMMON_PP_FOR_EACH_ARGS_13(m, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) \
    m(a1) m(a2) m(a3) m(a4) m(a5) m(a6) m(a7) m(a8) m(a9) m(a10) m(a11) m(a12) m(a13)
#define COMMON_PP_FOR_EACH_ARGS_14(m, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) \
    m(a1) m(a2) m(a3) m(a4) m(a5) m(a6) m(a7) m(a8) m(a9) m(a10) m(a11) m(a12) m(a13) m(a14)
#define COMMON_PP_FOR_EACH_ARGS_15(m, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) \
    m(a1) m(a2) m(a3) m(a4) m(a5) m(a6) m(a7) m(a8) m(a9) m(a10) m(a11) m(a12) m(a13) m(a14) m(a15)


#endif // COMMON_BASE_PREPROCESS_VARARGS_H
