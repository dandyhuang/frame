//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 20:03
//  @file:      ref_counted.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/ref_counted.h"

#include <assert.h>
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/atomic.h"

namespace common {

bool IRefCounted::IsUnique() const
{
    return AtomicGet(&m_ref_count) == 1;
}

int IRefCounted::GetRefCount() const
{
    return AtomicGet(&m_ref_count);
}

IRefCounted::IRefCounted() : m_ref_count(0)
{
}

IRefCounted::IRefCounted(int base_ref_count) : m_ref_count(base_ref_count)
{
}

IRefCounted::~IRefCounted()
{
}

int IRefCounted::AddRef() const
{
    return AtomicIncrement(&m_ref_count);
}

bool IRefCounted::Release() const
{
    int count = AtomicDecrement(&m_ref_count);
    assert(count >= 0);
    return count == 0;
}

} // end of namespace common
