//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:19
//  @file:      base_thread.cpp
//  @author:
//  @brief:
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/base_thread.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thirdparty/monitor_sdk_common/system/check_error.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/atomic.h"


namespace common {

size_t BaseThread::s_DefaultStackSize;

void BaseThread::SetStackSize(size_t size)
{
    m_StackSize = size;
}

BaseThread::BaseThread():
    m_Handle(),
    m_Id(-1),
    m_StackSize(s_DefaultStackSize),
    m_bStopRequested(false),
    m_IsAlive(false)
{
    Initialize();
}

void BaseThread::SetDefaultStackSize(size_t size)
{
    s_DefaultStackSize = size;
}

void BaseThread::OnExit()
{
    m_IsAlive = false;
}

#ifdef __unix__
///////////////////////////////////////////////////////////////////////////////
// Posix implementation

pthread_once_t BaseThread::s_InitializeOnce = PTHREAD_ONCE_INIT;
pthread_key_t BaseThread::s_nMainKey = 0;

void BaseThread::Initialize()
{
    CHECK_PTHREAD_ERROR(pthread_once(&s_InitializeOnce, DoInitialize));
}

void BaseThread::DoInitialize()
{
    CHECK_PTHREAD_ERROR(pthread_key_create(&BaseThread::s_nMainKey, NULL));
}

BaseThread::~BaseThread()
{
    // What we're trying to do is allow the thread we want to delete to complete
    // running. So we wait for it to stop.
    if (IsJoinable())
        Join();
}

bool BaseThread::Start()
{
    // Start can only be used on a new born or a started but already joined
    // or a detached Thread object. In other words, not associated to any
    // system thread, both alive and dead.
    if (IsJoinable())
    {
        // If crash here, means the thread is still alive or finished but not
        // joined.
        CHECK_ERRNO_ERROR(EINVAL);
    }

    m_Handle = ThreadHandleType();
    m_Id = 0;
    m_bStopRequested = false;

    pthread_attr_t attr;
    CHECK_PTHREAD_ERROR(pthread_attr_init(&attr));
    if (m_StackSize)
        CHECK_PTHREAD_ERROR(pthread_attr_setstacksize(&attr, m_StackSize));
    int error = pthread_create(&m_Handle, &attr, StaticEntry, this);
    if (error != 0) {
        if (error != EAGAIN) {
            CHECK_PTHREAD_ERROR(error);
        }
        m_Id = -1;
    }
    CHECK_PTHREAD_ERROR(pthread_attr_destroy(&attr));
    return error == 0;
}

bool BaseThread::StopAndWaitForExit()
{
    if (IsJoinable())
    {
        m_bStopRequested = true;
        return Join();
    }
    return false;
}

bool BaseThread::DoDetach()
{
    CHECK_PTHREAD_ERROR(pthread_detach(m_Handle));
    m_Handle = HandleType();
    m_Id = -1;
    return true;
}

bool BaseThread::Join()
{
    assert(IsJoinable());
    CHECK_PTHREAD_ERROR(pthread_join(m_Handle, NULL));
    m_Handle = pthread_t();
    m_Id = -1;
    return true;
}

int BaseThread::GetId() const
{
    if (m_Id != 0)
        return m_Id;

    // GetId is rarely used, so busy wait is more fitness
    while (AtomicGet(&m_Id) == 0)
        ThisThread::Sleep(1);

    return m_Id;
}

bool BaseThread::IsAlive() const
{
    return m_IsAlive;
}

bool BaseThread::IsJoinable() const
{
    return !pthread_equal(m_Handle, pthread_t());
}

// make sure execute before exit
void BaseThread::Cleanup(void* param)
{
    BaseThread* thread = static_cast<BaseThread*>(param);
    thread->OnExit();
}

void* BaseThread::StaticEntry(void* inBaseThread)
{
    BaseThread* theBaseThread = static_cast<BaseThread*>(inBaseThread);
    theBaseThread->m_IsAlive = true;
    theBaseThread->m_Id = ThisThread::GetId();
    pthread_setspecific(BaseThread::s_nMainKey, theBaseThread);

    pthread_cleanup_push(Cleanup, inBaseThread);
    theBaseThread->Entry();
    theBaseThread->m_IsAlive = false;
    pthread_cleanup_pop(true);

    return 0;
}

BaseThread* BaseThread::GetCurrent()
{
    return static_cast<BaseThread *>(pthread_getspecific(BaseThread::s_nMainKey));
}

#endif // __unix__

//////////////////////////////////////////////////////////////////////////////
// Windows Implementation
#ifdef  _WIN32
#include <process.h>
#include "thirdparty/monitor_sdk_common/base/common_windows.h"

uint32_t BaseThread::s_nThreadStorageIndex = 0;

void BaseThread::Initialize()
{
    static int s_InitializeOnce = (DoInitialize(), 1);
    (void) s_InitializeOnce;
}

void BaseThread::DoInitialize()
{
    s_nThreadStorageIndex = ::TlsAlloc();
    assert(s_nThreadStorageIndex >= 0);
}

BaseThread::~BaseThread()
{
    CloseHandle(m_Handle);
}

bool BaseThread::Start()
{
    unsigned int tid = 0;
    m_bStopRequested = false;
    m_Handle = (HANDLE)_beginthreadex(NULL, s_DefaultStackSize, StaticEntry, this, 0, &tid);
    m_Id = tid;
    return m_Handle != NULL;
}

bool BaseThread::StopAndWaitForExit()
{
    m_bStopRequested = true;
    if (IsJoinable())
        return Join();
    return false;
}

bool BaseThread::DoDetach()
{
    CloseHandle(m_Handle);
    m_Handle = NULL;
    m_Id = -1;
    return true;
}

bool BaseThread::Join()
{
    assert(IsJoinable());
    if (m_Handle)
    {
        DWORD error_code = ::WaitForSingleObject(m_Handle, INFINITE);
        m_Handle = NULL;
        m_Id = -1;
        return error_code == WAIT_OBJECT_0;
    }
    return false;
}

int BaseThread::GetId() const
{
    return m_Id;
}

bool BaseThread::IsAlive() const
{
    return WaitForSingleObject(m_Handle, 0) == WAIT_TIMEOUT;
}

bool BaseThread::IsJoinable() const
{
    return m_Handle != NULL;
}

unsigned int __stdcall BaseThread::StaticEntry(void* inBaseThread)
{
    BaseThread* theBaseThread = static_cast<BaseThread*>(inBaseThread);
    ::TlsSetValue(s_nThreadStorageIndex, theBaseThread);
    theBaseThread->Entry();
    return 0;
}

BaseThread* BaseThread::GetCurrent()
{
    return static_cast<BaseThread *>(::TlsGetValue(s_nThreadStorageIndex));
}

#endif // _WIN32

} // end of namespace common

