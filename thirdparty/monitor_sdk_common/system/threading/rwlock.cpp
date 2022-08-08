// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "thirdparty/monitor_sdk_common/system/threading/rwlock.h"

#include <string.h>

namespace common {

RwLock::RwLock()
{
    // Note: default rwlock is prefer reader
    COMMON_CHECK_PTHREAD_ERROR(pthread_rwlock_init(&m_lock, NULL));
}

RwLock::RwLock(Kind kind)
{
    pthread_rwlockattr_t attr;
    COMMON_CHECK_PTHREAD_ERROR(pthread_rwlockattr_init(&attr));
    COMMON_CHECK_PTHREAD_ERROR(pthread_rwlockattr_setkind_np(&attr, kind));
    COMMON_CHECK_PTHREAD_ERROR(pthread_rwlock_init(&m_lock, &attr));
    COMMON_CHECK_PTHREAD_ERROR(pthread_rwlockattr_destroy(&attr));
}

RwLock::~RwLock()
{
    COMMON_CHECK_PTHREAD_ERROR(pthread_rwlock_destroy(&m_lock));
    memset(&m_lock, 0xFF, sizeof(m_lock));
}

} // namespace common
