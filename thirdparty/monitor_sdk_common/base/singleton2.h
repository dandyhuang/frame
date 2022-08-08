//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 12:09
//  @file:      singleton.h
//  @author:
//  @brief:     singleton class template
//
//********************************************************************


#ifndef COMMON_BASE_SINGLETON_H_INCLUDED
#define COMMON_BASE_SINGLETON_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "thirdparty/monitor_sdk_common/base/platform_features.h"
#include "thirdparty/monitor_sdk_common/base/uncopyable.h"

/*
// example: define a singleton class
class TestClass : public SingletonBase<TestClass>
{
    friend class SingletonBase<TestClass>;
private:
    TestClass() {}
    ~TestClass() {}
public:
    int Test() const
    {
        return 1;
    }
};

// example2: define a singleton class with alt access method
// private inherit make 'Instance' method unaccessable
class TestClass2 : private SingletonBase<TestClass2>
{
    friend class SingletonBase<TestClass2>;
private:
    TestClass() {}
    ~TestClass() {}
public:
    // using DefaultInstance to instead Instance
    static TestClass2& DefaultInstance()
    {
        return Instance();
    }
};

// example3: make a singleton for class
class TestClass3
{
};

typedef Singleton<TestClass3> SingletonTestClass3;
TestClass3& instance = SingletonTestClass3::Instance();

*/

// #undef HAS_THREAD_SAFE_STATICS

#if HAS_THREAD_SAFE_STATICS
namespace common {

template <typename T>
class SingletonBase
{
    DECLARE_UNCOPYABLE(SingletonBase);

private:
    struct Holder
    {
        T value;
        bool is_alive;

        explicit Holder(Holder** holder) : value(), is_alive(true)
        {
            *holder = this;
        }

        template <typename A1>
        Holder(Holder** holder, A1& a1) : value(a1), is_alive(true)
        {
            *holder = this;
        }

        ~Holder()
        {
            is_alive = false;
        }
    };

protected:
    SingletonBase() {}
    ~SingletonBase() {}

public:
    static T& Instance()
    {
        static Holder holder(&s_holder);
        if (!holder.is_alive) {
            fprintf(stderr, "Singleton has been destroyed\n");
            abort();
        }
        return holder.value;
    }

    // 获取带构造参数的单体对象时，只有第一次调用Instance的参数有效
    template <typename A1>
    static T& Instance(A1& a1)
    {
        static Holder holder(&s_holder, a1);
        if (!holder.is_alive) {
            fprintf(stderr, "Singleton has been destroyed\n");
            abort();
        }
        return holder.value;
    }

    static bool IsAlive()
    {
        return s_holder && s_holder->is_alive;
    }

private:
    static Holder* s_holder;
};

template <typename T>
typename SingletonBase<T>::Holder* SingletonBase<T>::s_holder;


} // end of namespace common

#else

#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"
#include "thirdparty/monitor_sdk_common/system/memory/barrier.h"

namespace common {

template <typename T>
class SingletonBase
{
    COMMON_DECLARE_UNCOPYABLE(SingletonBase);

protected:
    SingletonBase() {}
    ~SingletonBase() {}

public:
    static T& Instance()
    {
        if (!m_is_alive) {
            fprintf(stderr, "Singleton has been destroyed\n");
            abort();
        }

        // double check locking optimize
        if (!m_instance)
        {
            ScopedLocker<Mutex> lock(GetSingletonLock());
            if (!m_instance)
            {
                T* p = new T();
                MemoryWriteBarrier();
                m_instance = p;
                atexit(Destroy);
            }
        }
        return *m_instance;
    }

    template <typename A1>
    static T& Instance(A1& a1)
    {
        if (!m_is_alive) {
            fprintf(stderr, "Singleton has been destroyed\n");
            abort();
        }

        // double check locking optimize
        if (!m_instance)
        {
            ScopedLocker<Mutex> lock(GetSingletonLock());
            if (!m_instance)
            {
                T* p = new T(a1);
                MemoryWriteBarrier();
                m_instance = p;
                atexit(Destroy);
            }
        }
        return *m_instance;
    }

    static bool IsAlive()
    {
        return m_instance != NULL;
    }

    // 模板类因为编译器不确定类型T的实例，不能使用friend，所以这里使用了public方法，
    // 安全起见不要使用这个方法，这个方法是给SingletonMocker使用的
    // 返回之前的指针
    static T* Reset(T* mock_ptr) {
        // 首先要有一个Instance
        Instance();
        T* old = m_instance;
        m_instance = mock_ptr;
        return old;
    }

private:
    static void Destroy()
    {
        m_is_alive = false;
        // need not locking
        if (m_instance)
        {
            delete m_instance;
            m_instance = NULL;
        }
    }

    // TODO() refine it. (ugly implement)
    static Mutex& GetSingletonLock() {
#ifdef _WIN32
        static Mutex local_lock;
        return local_lock;
#else
        return m_lock;
#endif
    }

private:
    static Mutex m_lock;
    static T* volatile m_instance;
    static bool volatile m_is_alive;
};

template <typename T>
class SingletonMocker {
public:
    SingletonMocker(T* new_mocker_ptr) {
        m_old = SingletonBase<T>::Reset(new_mocker_ptr);
    }
    ~SingletonMocker() {
        SingletonBase<T>::Reset(m_old);
    }

private:
    static T* volatile m_old;
};

template <typename T>
Mutex SingletonBase<T>::m_lock;

template <typename T>
T* volatile SingletonBase<T>::m_instance;

template <typename T>
T* volatile SingletonMocker<T>::m_old;

template <typename T>
bool volatile SingletonBase<T>::m_is_alive = true;

} // end of namespace common

#endif

namespace common {

template <typename T>
class Singleton : public SingletonBase<T>
{
};

} // end of namespace common

#endif // COMMON_BASE_SINGLETON_H_INCLUDED
