// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 05/31/11

#include "thirdparty/monitor_sdk_common/system/threading/thread_types.h"
#include <string>
#include "thirdparty/monitor_sdk_common/system/check_error.h"

namespace common {

ThreadAttributes& ThreadAttributes::SetName(const std::string& name) {
    m_name = name;
    return *this;
}

ThreadAttributes::ThreadAttributes() {
    COMMON_CHECK_PTHREAD_ERROR(pthread_attr_init(&m_attr));
}

ThreadAttributes::~ThreadAttributes() {
    COMMON_CHECK_PTHREAD_ERROR(pthread_attr_destroy(&m_attr));
}

ThreadAttributes& ThreadAttributes::SetStackSize(size_t size) {
    COMMON_CHECK_PTHREAD_ERROR(pthread_attr_setstacksize(&m_attr, size));
    return *this;
}

ThreadAttributes& ThreadAttributes::SetDetached(bool detached) {
    int state = detached ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE;
    COMMON_CHECK_PTHREAD_ERROR(pthread_attr_setdetachstate(&m_attr, state));
    return *this;
}

ThreadAttributes& ThreadAttributes::SetPriority(int priority) {
    return *this;
}

bool ThreadAttributes::IsDetached() const {
    int state = 0;
    COMMON_CHECK_PTHREAD_ERROR(pthread_attr_getdetachstate(&m_attr, &state));
    return state == PTHREAD_CREATE_DETACHED;
}

} // namespace common

