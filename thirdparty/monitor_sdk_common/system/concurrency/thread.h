//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 18:09
//  @file:      thread.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_THREAD_H_INCLUDED
#define COMMON_SYSTEM_CONCURRENCY_THREAD_H_INCLUDED

#include "thirdparty/monitor_sdk_common/base/closure2.h"
#include "thirdparty/monitor_sdk_common/base/deprecate.h"
#include "thirdparty/monitor_sdk_common/base/function.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/thread_types.h"

// Add this for compatible old usage, and should be removed in the future
#include "thirdparty/monitor_sdk_common/system/concurrency/this_thread.h"

namespace common {

/// �����Ա����תΪ��ͨ�����������ص���
/// @param Class ����
/// @param Member ��Ա������
/// ������ڳ�Ա������@code
/// void Test::XxxThread();
/// @endcode
/// �� MAKE_MEMBER_CALLBACK(Test, XxxThread)���ȿ����ɿ������̵߳ĺ���ָ�룬����Ϊ@code
/// void (*)(void* object, void* param)
/// @endcode
/// ע�⿪ͷ����һ���������������� this ָ�롣param ���ԡ�
#define MAKE_THREAD_CALLBACK(Class, Member) &::common::GenericMemberFunctionAdapter<Class, &Class::Member>

/// �����Ա�����õ�ͨ�õ��̺߳���
template<typename Class, void (Class::*Member)()>
void GenericMemberFunctionAdapter(void* context, unsigned long long param)
{
    (static_cast<Class*>(context)->*Member)();
}

/// �Ѵ�һ�����������Ա����תΪ��ͨ�����������ص���
/// @param Class ����
/// @param Member ��Ա������
/// @param ParamType ��Ա�����Ĳ������ͣ�ֻ����ָ����� intptr_t/uintptr_t
/// ������ڳ�Ա������@code
/// void Test::XxxThread(void* param);
/// @endcode
/// �� MAKE_MEMBER_CALLBACK(Test, XxxThread)���ȿ����ɿ������̵߳ĺ���ָ�룬����Ϊ@code
/// void (*)(void* object, void* param)
/// @endcode
/// ע�⿪ͷ����һ���������������� this ָ�롣
#define MAKE_PARAMETERIZED_THREAD_CALLBACK(Class, Member, ParamType) \
    &::common::GenericParamMemberFunctionAdapter<Class, ParamType, &Class::Member>
template<typename Class, typename ParamType, void (Class::*Member)(ParamType)>
void GenericParamMemberFunctionAdapter(void* context, unsigned long long param)
{
    (static_cast<Class*>(context)->*Member)(ParamType(param));
}

// Thread ��
// ���û�ֱ�Ӵ����̺߳���ʹ��
class Thread
{
    COMMON_DECLARE_UNCOPYABLE(Thread);
    class Impl;
public:
    typedef void (*StartRoutine)(void* context, unsigned long long param);

    // Ĭ�Ϲ�����̶߳���
    // ����� Initialize ����� Start
    Thread();

    explicit Thread(
        StartRoutine start_routine,
        void* context = NULL,
        unsigned long long param = 0
    );

    explicit Thread(Closure<void>* closure);
    explicit Thread(const Function<void ()>& function);

    // TODO(): Thread �಻Ӧ�ñ��������࣬����ֻ��Ϊ����Ϻ����
    // Entry��������Ҫȥ����
    virtual ~Thread();

    // ��ʼ���̶߳���
    void Initialize(StartRoutine start_routine, void* context = NULL, unsigned long long param = 0);
    void Initialize(Closure<void>* closure);
    void Initialize(const Function<void ()>& function);

    // ֻ���� Start ֮ǰ����
    void SetStackSize(size_t size);

    // ������� Start ��Ż���������
    bool Start();

    // �ȴ��������е��߳̽���
    // ֻ���߳��Ѿ������ˣ���û�� Detach������ Join
    // ����߳��Ѿ��������������� true
    bool Join();

    // ��ʵ���̺߳��̶߳�����룬Detach ֮�󣬲�����ͨ���κκ������ʵ�ʵ���߳�
    void Detach();

    void SendStopRequest();
    bool IsStopRequested() const;
    bool StopAndWaitForExit();

    // �����߳��Ƿ��ڴ��
    bool IsAlive() const;
    DEPRECATED_BY(IsAlive) bool IsRunning() const { return IsAlive(); }

    // �����Ƿ���Զ��̵߳� Join
    bool IsJoinable() const;

    // ���ϵͳ�����߳� handle�����������뺬�壬ƽ̨���
    ThreadHandleType GetHandle() const;

    // ����߳�ID������PID��������
    int GetId() const;

private:
    // Thread ����������� BaseThread �࣬BaseThread ���� override
    // Entry �����ķ�ʽʹ�ã����� Thread ����Ҫ֧�� Detach����˲��ٴ�
    // BaseThread ������Thread �౾����Ӧ����ֱ���Ժ���Ϊ������ʱ������ʹ�õġ�
    // ������������ﱨ��˵�����ô����ࡣ
    // ��������� BaseThread::Entry ǩ����һ����ʹ�ñ��������Լ�鵽���ִ���
    virtual void Entry() const {}

private:
    Impl* m_pimpl;
};

} // end of namespace common

#endif // COMMON_SYSTEM_CONCURRENCY_THREAD_H_INCLUDED

