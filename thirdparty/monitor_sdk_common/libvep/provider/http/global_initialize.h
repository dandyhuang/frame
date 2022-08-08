// Copyright (c) 2020, VIVO Inc. All rights reserved.                                                                                  |  1
// Author: Fly Qiu <rock3qiu@gmail.com>                                                                                                 |~
// Created: 2020-01-09                                                                                                                    |~

#ifndef COMMON_LIBVEP_GLOBAL_INITIALIZE_H_
#define COMMON_LIBVEP_GLOBAL_INITIALIZE_H_

/// global initialize, run some code before main
/// @param name module name
#define GLOBAL_INITIALIZE(name) \
static void name##_init(); \
static bool call_global_##name##_init() { \
    name##_init(); \
    return true; \
} \
bool name##_initialized = call_global_##name##_init(); \
static void name##_init()

#if 0

// example
GLOBAL_INITIALIZE(this_test) {
    FLAGS_gtest_death_test_style = "threadsafe";
    print("test initialized");
}

#endif

#endif // COMMON_LIBVEP_GLOBAL_INITIALIZE_H_
