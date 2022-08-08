//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-22 14:48
//  @file:      memory_cache.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_COLLECTION_MEMORY_CACHE_H
#define COMMON_COLLECTION_MEMORY_CACHE_H
#pragma once

#include <map>
#include "thirdparty/monitor_sdk_common/base/container_of.h"
#include "thirdparty/monitor_sdk_common/base/stdext/intrusive_list.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/rwlock.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"
#include "thirdparty/monitor_sdk_common/system/time/timestamp.h"

namespace common {

template <typename Key, typename Value>
class MemoryCache
{
    COMMON_DECLARE_UNCOPYABLE(MemoryCache);
protected:
    struct Node
    {
        Value value;
        int64_t expiry_time_in_ms;
        mutable list_node link;
    };
    // TODO(): using ClosedHashMap to instead std::map
    typedef std::map<Key, Node> MapType;
public:
    explicit MemoryCache(size_t capacity) :
        m_capacity(capacity)
    {
    }

    virtual ~MemoryCache(){}

    size_t Capacity() const
    {
        return m_capacity;
    }

    size_t Size() const
    {
        typename RWLock::ReaderLocker locker(m_lock);
        return m_map.size();
    }

    bool IsEmpty() const
    {
        typename RWLock::ReaderLocker locker(m_lock);
        return m_map.empty();
    }

    bool IsFull() const
    {
        typename RWLock::ReaderLocker locker(m_lock);
        return m_map.size() == m_capacity;
    }

    /// return false if key alread existed
    bool Insert(const Key& key, const Value& value)
    {
        return InsertWithExpiry(key, value, kVeryLongTimeInMs);
    }

    /// return false if key alread existed
    bool InsertWithExpiry(const Key& key,
                          const Value& value,
                          int64_t life_cycle_in_ms)
    {
        typename RWLock::WriterLocker locker(m_lock);

        typename MapType::iterator it = m_map.find(key);
        if (it != m_map.end()) {
            it->second.expiry_time_in_ms = GetTimeStampInMs() + life_cycle_in_ms;
            return false;
        }

        Node& node = m_map[key];
        node.value = value;
        node.expiry_time_in_ms = GetTimeStampInMs() + life_cycle_in_ms;
        node.link.init();
        MarkAsHot(&node);
        if (m_map.size() > m_capacity)
            DiscardWithinLock(1);
        return true;
    }

    /// return false if key doesn't exist
    bool Replace(const Key& key, const Value& value)
    {
        return ReplaceWithExpiry(key, value, kVeryLongTimeInMs);
    }

    /// return false if key doesn't exist
    bool ReplaceWithExpiry(const Key& key, const Value& value, int64_t life_cycle_in_ms)
    {
        typename RWLock::WriterLocker locker(m_lock);
        typename MapType::iterator it = m_map.find(key);
        if (it == m_map.end())
            return false;
        it->second.value = value;
        it->second.expiry_time_in_ms = GetTimeStampInMs() + life_cycle_in_ms;
        MarkAsHot(&it->second);
        return true;
    }

    /// insert if not exist, replace if exist
    bool Put(const Key& key, const Value& value)
    {
        return PutWithExpiry(key, value, kVeryLongTimeInMs);
    }

    /// insert if not exist, replace if exist
    bool PutWithExpiry(const Key& key, const Value& value, int64_t life_cycle_in_ms)
    {
        typename RWLock::WriterLocker locker(m_lock);
        Node& node = m_map[key];
        node.value = value;
        node.expiry_time_in_ms = GetTimeStampInMs() + life_cycle_in_ms;
        MarkAsHot(&node);
        if (m_map.size() > m_capacity)
            DiscardWithinLock(1);
        return true;
    }

    /// @retval false if not found
    bool Get(const Key& key, Value* value) const
    {
        typename RWLock::ReaderLocker locker(m_lock);
        typename MapType::const_iterator it = m_map.find(key);
        if (it == m_map.end())
            return false;

        if (it->second.expiry_time_in_ms < GetTimeStampInMs())
            return false;
        *value = it->second.value;
        MarkAsHot(&it->second);
        return true;
    }

    /// get value or return default value if not exist
    Value GetOrDefault(const Key& key, const Value& default_value = Value()) const
    {
        Value value(default_value);
        if (Get(key, &value))
            return value;
        return value;
    }

    /// @return whether found
    bool Contains(const Key& key) const
    {
        typename RWLock::ReaderLocker locker(m_lock);
        typename MapType::const_iterator it = m_map.find(key);
        if (it == m_map.end())
            return false;
        if (it->second.expiry_time_in_ms < GetTimeStampInMs())
            return false;
        return true;
    }

    bool Remove(const Key& key)
    {
        typename RWLock::WriterLocker locker(m_lock);
        return m_map.erase(key) == 1;
    }

    void Clear()
    {
        typename RWLock::WriterLocker locker(m_lock);
        m_map.clear();
        assert(m_lru_list.empty());
    }

    /// iteration: return first element
    bool First(Key* key, Value* value) const
    {
        typename RWLock::ReaderLocker locker(m_lock);
        return FindNextWithinExpiry(m_map.begin(), key, value);
    }

    /// iteration: return next element
    bool Next(Key* key, Value* value) const
    {
        typename RWLock::ReaderLocker locker(m_lock);
        return FindNextWithinExpiry(m_map.upper_bound(*key), key, value);
    }

    /// discard count elements forcely
    /// @return number of elements discarded
    size_t Discard(size_t count)
    {
        typename RWLock::WriterLocker locker(m_lock);
        return DiscardWithinLock(count);
    }

    void Dump(::std::map<Key, Value>* map_ptr) const {
        map_ptr->clear();
        typename RWLock::ReaderLocker locker(m_lock);
        for (typename MapType::const_iterator iter = m_map.begin();
                iter != m_map.end(); ++iter) {
            (*map_ptr)[iter->first] = iter->second.value;
        }
    }

protected:
    static const int64_t kVeryLongTimeInMs = 200LL * 365 * 24 * 60 * 60 * 1000;
    void MarkAsHot(const Node* node) const
    {
        typename Mutex::Locker locker(m_lru_lock);
        if (!m_lru_list.empty() && node == &m_lru_list.back())
            return;
        node->link.unlink();
        m_lru_list.push_back(*const_cast<Node*>(node));
    }

    size_t DiscardWithinLock(size_t count)
    {
        size_t discarded = 0;
        while (discarded < count && !m_lru_list.empty())
        {
            typename intrusive_list<Node>::iterator it = m_lru_list.begin();
            Node* node = &*it;
            typename MapType::value_type* pair =
                container_of(node, typename MapType::value_type, second);
            m_map.erase(pair->first);
            ++discarded;
        }
        return discarded;
    }

    bool FindNextWithinExpiry(typename MapType::const_iterator start, Key* key, Value* value) const
    {
        int64_t current_time_in_ms = GetTimeStampInMs();
        for (typename MapType::const_iterator it = start;
             it != m_map.end(); ++it)
        {
            if (it->second.expiry_time_in_ms < current_time_in_ms)
                continue;
            *key = it->first;
            *value = it->second.value;
            return true;
        }
        return false;
    }
protected:
    mutable RWLock m_lock;
    mutable Mutex m_lru_lock;
    size_t m_capacity;
    MapType m_map;
    mutable intrusive_list<Node> m_lru_list;
};

} // end of namespace common

#endif // COMMON_COLLECTION_MEMORY_CACHE_H
