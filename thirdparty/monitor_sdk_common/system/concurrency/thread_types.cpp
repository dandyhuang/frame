//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 18:24
//  @file:      thread_types.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/thread_types.h"

namespace common {

ThreadAttribute& ThreadAttribute::SetName(const std::string& name)
{
    m_name = name;
    return *this;
}

#ifdef __unix__
ThreadAttribute::ThreadAttribute()
{
    pthread_attr_init(&m_attr);
}

ThreadAttribute::~ThreadAttribute()
{
    pthread_attr_destroy(&m_attr);
}

ThreadAttribute& ThreadAttribute::SetStackSize(size_t size)
{
    pthread_attr_setstacksize(&m_attr, size);
    return *this;
}

ThreadAttribute& ThreadAttribute::SetDetached(bool detached)
{
    pthread_attr_setdetachstate(&m_attr, detached);
    return *this;
}

ThreadAttribute& ThreadAttribute::SetPriority(int priority)
{
    return *this;
}
#endif // __unix__

#ifdef _WIN32

#endif

} // end of namespace common
