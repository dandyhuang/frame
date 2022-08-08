// Copyright (c) 2015, Vivo Inc. All rights reserved
/**
 * @file object_pool.h
 * @brief
 * @author chenzheng05@baidu.com
 * @date 2015-01-20
 */
#ifndef COMMON_BASE_OBJECT_POOL_H
#define COMMON_BASE_OBJECT_POOL_H

#include <limits.h>
#include <algorithm>
#include <vector>

#include "thirdparty/monitor_sdk_common/base/deprecate.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/spinlock.h"

#undef min
#undef max

/// @file ObjectType.hpp
/// @brief ������ ObjectPool �� FixedObjectPool ������ģ��
/// ����ؼ�������Ĺ��������������Ϊ������ͬʱ���ڵĶ��������������������
/// ����ؿ���ָ����շ�ʽ��Ҫ����һ�� static �� Clear ��Ա����������Ϊ����
/// ��ָ�����͡�
///
/// �����Ǽ������õ���շ���

// namespace common {

struct ObjectPool_DefaultAllocator
{
    template <typename T>
    static T* New(T* hint)
    {
        return new T();
    }

    template <typename T>
    static void Clear(T* p)
    {
    }

    template <typename T>
    static void Delete(T* p)
    {
        delete p;
    }
};

/// ��շ�ʽ������ Clear ��Ա����
struct ObjectPool_NormalAllocator : public ObjectPool_DefaultAllocator
{
    template <typename T>
    static void Clear(T* p)
    {
        p->Clear();
    }
};

/// ��շ�ʽ������ clear ��Ա�����������ڸ��� STL ����
struct ObjectPool_StdCxxAllocator : public ObjectPool_DefaultAllocator
{
    template <typename T>
    static void Clear(T* p)
    {
        p->clear();
    }
};

/// ֧�ֶ�̬�������Ķ����
template <typename T,
    typename Allocator = ObjectPool_DefaultAllocator,
    typename LockType = common::Spinlock
>
class ObjectPool
{
    typedef ObjectPool ThisType;
public:
    typedef T ObjectType;
public:
    explicit ObjectPool(
        size_t quota = INT_MAX,  ///< ��󻺴�����
        size_t initial_size = 0, ///< ��ʼ��С
        bool auto_create = true, ///< ��Ϊ��ʱ��������Ƿ��Զ�����
        Allocator allocator = Allocator()
    ):
        m_Quota(quota), m_AutoCreate(auto_create), m_Allocator(allocator)
    {
        Reserve(std::min(initial_size, quota));
    }

    ~ObjectPool()
    {
        Clear();
    }

    /// �������
    size_t GetQuota() const
    {
        return m_Quota;
    }

    size_t Size() const
    {
        typename LockType::Locker locker(m_Lock);
        return m_PooledObjects.size();
    }

    /// �������
    void SetQuota(size_t size)
    {
        typename LockType::Locker locker(m_Lock);
        m_Quota = size;
        UnlockedShrink(size);
    }

    /// Ԥ������
    /// @param size ����Ԥ���ĳ��ж���ĸ���
    void Reserve(size_t size)
    {
        if (size > m_Quota)
            size = m_Quota;
        typename LockType::Locker locker(m_Lock);
        while (m_PooledObjects.size() < size)
        {
            m_PooledObjects.push_back(NewObject());
        }
    }

    /// �Ӷ���ػ�ö���
    T* Acquire()
    {
        {
            typename LockType::Locker locker(m_Lock);
            if (m_PooledObjects.empty())
            {
                if (!m_AutoCreate)
                    return NULL;
            }
            else
            {
                T* p = m_PooledObjects.back();
                m_PooledObjects.pop_back();
                return p;
            }
        }
        return NewObject();
    }

    /// �黹����
    void Release(const T* p)
    {
        T* q = const_cast<T*>(p);
        m_Allocator.Clear(q);
        {
            typename LockType::Locker locker(m_Lock);
            if (m_PooledObjects.size() < m_Quota)
            {
                m_PooledObjects.push_back(q);
                return;
            }
        }
        m_Allocator.Delete(q);
    }

    /// ����������ﻺ��Ķ��󵽲����� size ��
    void Shrink(size_t size = 0)
    {
        typename LockType::Locker locker(m_Lock);
        UnlockedShrink(size);
    }

    /// ������л���Ķ���
    void Clear()
    {
        Shrink(0);
    }
private:
    void UnlockedShrink(size_t size)
    {
        while (m_PooledObjects.size() > size)
        {
            delete m_PooledObjects.back();
            m_PooledObjects.pop_back();
        }
    }

    T* NewObject()
    {
        return m_Allocator.New(static_cast<T*>(0));
    }

private:
    ObjectPool(const ObjectPool& src);
    ObjectPool& operator=(const ObjectPool& rhs);
private:
    mutable LockType m_Lock;
    std::vector<T*> m_PooledObjects;
    size_t m_Quota;
    bool m_AutoCreate;
    Allocator m_Allocator;
};

/// @brief �����ڼ�ָ�����Ķ����
/// @tparam T ��������
/// @tparam Quota ���õ����
/// @tparam Allocator ָ����ն���ķ���
template <
    typename T,
    size_t Quota,
    typename Allocator = ObjectPool_DefaultAllocator,
    typename LockType = common::Spinlock
>
class FixedObjectPool
{
    typedef FixedObjectPool ThisType;
public:
    typedef T ObjectType;
public:
    explicit FixedObjectPool(
        size_t initial_size = 0,
        bool auto_create = true,
        Allocator allocator = Allocator()
    ) :
        m_Count(0), m_AutoCreate(auto_create), m_Allocator(allocator)
    {
        Reserve(std::min(Quota, initial_size));
    }

    ~FixedObjectPool()
    {
        Clear();
    }

    /// �Ӷ���ػ�ö���
    T* Acquire()
    {
        {
            typename LockType::Locker locker(m_Lock);
            if (m_Count > 0)
            {
                return m_PooledObjects[--m_Count];
            }
        }
        if (m_AutoCreate)
            return NewObject();
        else
            return NULL;
    }

    /// �黹����
    void Release(const T* p)
    {
        T* q = const_cast<T*>(p);
        m_Allocator.Clear(q);
        {
            typename LockType::Locker locker(m_Lock);
            if (m_Count < Quota)
            {
                m_PooledObjects[m_Count++] = q;
                return;
            }
        }
        m_Allocator.Delete(q);
    }

    size_t Size() const
    {
        typename LockType::Locker locker(m_Lock);
        return m_Count;
    }

    /// Ԥ������
    /// @param size ����Ԥ���ĳ��ж���ĸ���
    void Reserve(size_t size)
    {
        if (size > Quota)
            size = Quota;
        typename LockType::Locker locker(m_Lock);
        while (m_Count < size)
        {
            T* p = NewObject();
            m_PooledObjects[m_Count++] = p;
        }
    }

    /// ��������Ķ���
    void Shrink(size_t size = 0)
    {
        typename LockType::Locker locker(m_Lock);
        UnlockedShrink(size);
    }

    /// ������л���Ķ���
    void Clear()
    {
        typename LockType::Locker locker(m_Lock);
        UnlockedShrink(0);
    }

private:
    FixedObjectPool(const FixedObjectPool& src);
    FixedObjectPool& operator=(const FixedObjectPool& rhs);
    T* NewObject()
    {
        return m_Allocator.New(static_cast<T*>(0));
    }

    /// ��������Ķ���
    void UnlockedShrink(size_t size)
    {
        while (m_Count > size)
        {
            m_Allocator.Delete(m_PooledObjects[--m_Count]);
        }
    }

private:
    mutable LockType m_Lock;
    T* m_PooledObjects[Quota];
    size_t m_Count;
    bool m_AutoCreate;
    Allocator m_Allocator;
};

DEPRECATED_BY(ObjectPool_DefaultAllocator)
typedef ObjectPool_DefaultAllocator NullClear;

DEPRECATED_BY(ObjectPool_StdCxxAllocator)
typedef ObjectPool_StdCxxAllocator CallMember_clear;

DEPRECATED_BY(ObjectPool_NormalAllocator)
typedef ObjectPool_NormalAllocator CallMember_Clear;

// } // namespace common

#endif // COMMON_BASE_OBJECT_POOL_H
