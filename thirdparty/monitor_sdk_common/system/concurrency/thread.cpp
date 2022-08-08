//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 18:10
//  @file:      thread.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/thread.h"
#include "thirdparty/monitor_sdk_common/system/check_error.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/base_thread.h"

#include "thirdparty/glog/logging.h"

namespace common {

class Thread::Impl : public ::common::BaseThread
{
public:
    Impl():
        m_StartRoutine(NULL),
        m_Context(NULL),
        m_Param(0),
        m_closure(NULL)
    {
    }

    explicit Impl(
        StartRoutine start_routine,
        void* context,
        unsigned long long param
    ):
        m_StartRoutine(start_routine),
        m_Context(context),
        m_Param(param),
        m_closure(NULL)
    {
    }

    explicit Impl(Closure<void>* closure) :
        m_StartRoutine(NULL),
        m_Context(NULL),
        m_Param(0),
        m_closure(closure)
    {
    }

    explicit Impl(const Function<void ()>& function) :
        m_StartRoutine(NULL),
        m_Context(NULL),
        m_Param(0),
        m_closure(NULL),
        m_function(function)
    {
    }

    ~Impl()
    {
        delete m_closure;

        // Reset all to null for debug purpose
        m_closure = NULL;
        m_StartRoutine = NULL;
        m_Context = NULL;
        m_Param = 0;
        m_function = NULL;
    }

    void Initialize(StartRoutine start_routine, void* context, unsigned long long param)
    {
        CheckNotJoinable();

        m_StartRoutine = start_routine;
        m_Context = context;
        m_Param = param;

        delete m_closure;
        m_closure = NULL;
        m_function = NULL;
    }

    void Initialize(Closure<void>* closure)
    {
        CheckNotJoinable();

        if (closure != m_closure)
        {
            delete m_closure;
            m_closure = closure;
        }

        m_StartRoutine = NULL;
        m_Context = NULL;
        m_Param = 0;
        m_function = NULL;
    }

    void Initialize(const Function<void ()>& function)
    {
        CheckNotJoinable();

        m_function = function;

        delete m_closure;
        m_StartRoutine = NULL;
        m_Context = NULL;
        m_Param = 0;
        m_closure = NULL;
    }

    bool IsInitialized() const
    {
        return m_StartRoutine != NULL || m_closure != NULL || m_function != NULL;
    }

private:
    virtual void Entry()
    {
        if (m_function) {
            m_function();
        } else if (m_closure) {
            Closure<void>* closure = m_closure;
            if (closure->IsSelfDelete())
                m_closure = NULL;
            closure->Run();
        } else if (m_StartRoutine) {
            m_StartRoutine(m_Context, m_Param);
        }
    }

    virtual void OnExit()
    {
        if (!IsJoinable())
            delete this;
        else
            BaseThread::OnExit();
    }

    void CheckNotJoinable() const
    {
        // Can't reinitialze a joinable thread
        if (IsJoinable())
            CHECK_ERRNO_ERROR(EINVAL);
    }

private:
    // Description about the routine.
    StartRoutine m_StartRoutine;
    void* m_Context;
    unsigned long long m_Param;

    // Description about the routine by Closure.
    // If it's set, ignore the above routine.
    Closure<void>* m_closure;

    // Description about the routine by Function.
    // If it's set, ignore the above routine.
    Function<void ()> m_function;
};


Thread::Thread(): m_pimpl(new Impl())
{
}

Thread::Thread(
    StartRoutine start_routine,
    void* context,
    unsigned long long param
) : m_pimpl(new Impl(start_routine, context, param))
{
}

Thread::Thread(Closure<void>* closure) : m_pimpl(new Impl(closure))
{
}

Thread::Thread(const Function<void ()>& function) :
    m_pimpl(new Impl(function))
{
}

Thread::~Thread()
{
    delete m_pimpl;
    m_pimpl = NULL;
}

void Thread::Initialize(
    StartRoutine start_routine,
    void* context,
    unsigned long long param)
{
    m_pimpl->Initialize(start_routine, context, param);
}

void Thread::Initialize(Closure<void>* closure)
{
    m_pimpl->Initialize(closure);
}

void Thread::Initialize(const Function<void ()>& function)
{
    m_pimpl->Initialize(function);
}

void Thread::SetStackSize(size_t size)
{
    return m_pimpl->SetStackSize(size);
}

bool Thread::Start()
{
    if (!m_pimpl->IsInitialized())
        CHECK_ERRNO_ERROR(EINVAL);
    return m_pimpl->Start();
}

bool Thread::Join()
{
    return m_pimpl->Join();
}

void Thread::Detach()
{
    // After detached, the m_pimpl will be deleted in Thread::Impl::OnExit.
    // So don't delete it here, just set it be NULL to mark this thread object
    // to be detached.
    m_pimpl->DoDetach();
    m_pimpl = NULL;
}

void Thread::SendStopRequest()
{
    m_pimpl->SendStopRequest();
}

bool Thread::IsStopRequested() const
{
    return m_pimpl->IsStopRequested();
}

bool Thread::StopAndWaitForExit()
{
    return m_pimpl->StopAndWaitForExit();
}

bool Thread::IsAlive() const
{
    return m_pimpl && m_pimpl->IsAlive();
}

bool Thread::IsJoinable() const
{
    return m_pimpl && m_pimpl->IsJoinable();
}

ThreadHandleType Thread::GetHandle() const
{
    return m_pimpl->GetHandle();
}

int Thread::GetId() const
{
    return m_pimpl->GetId();
}

} // end of namespace common

