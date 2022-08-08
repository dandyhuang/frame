//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-12-09 16:41
//  @file:      mempool.h
//  @author:    
//  @brief:     
//
//********************************************************************

#ifndef COMMON_SYSTEM_MEMORY_MEMPOOL_H
#define COMMON_SYSTEM_MEMORY_MEMPOOL_H

#include <assert.h>
#include <deque>
#include "thirdparty/monitor_sdk_common/base/deprecate.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/spinlock.h"


namespace common {

class MemPool
{
    struct BLOCK_HEADER;
public:
    static const size_t MIN_UNIT_SIZE = 1024;
    static const size_t MAX_UNIT_SIZE = 16 * 2048 * MIN_UNIT_SIZE;
    static const int MAX_INDEX = 16;

public:
    explicit MemPool(size_t dwMaxMemSize);

    ~MemPool();

    // ��������:             ����һ����СΪdwSize��MemUnit
    // �������:             @����1: dwSize; MemUnit�Ĵ�С;
    // ����ֵ:               MemUnit��ָ��;
    void* Allocate(size_t dwSize);

    // ��������:             �ͷ�һ��MemUnit����Դ
    // �������:             @����1: pUnit, �ڴ���ָ��;
    // ����ֵ:               ��
    void Free(void* p);

    size_t GetBlockSize(const void* ptr) const;
    size_t GetAllocatedSize() const;
    size_t GetPooledSize() const;
    size_t GetAllocateCount() const;
    size_t GetFreeCount() const;
    size_t GetNewCount() const;
    size_t GetDeleteCount() const;

private:
    BLOCK_HEADER* AllocateBlock(unsigned int index);
    BLOCK_HEADER* AllocateLargeBlock(size_t size);
    void FreeBlock(BLOCK_HEADER* block);
    bool IsValidMemoryBlock(const BLOCK_HEADER* block_header);
    int GetUnitIndex(size_t dwSize);
private:
    Spinlock                m_poolMutex;
    std::deque<BLOCK_HEADER*>   m_memPool[MAX_INDEX];
    size_t m_dwQueueNum;
    size_t m_allocated_memory_size;
    size_t m_pooled_memory_size;
    size_t m_dwAllocateCount;
    size_t m_dwFreeCount;
    size_t m_dwNewCount;
    size_t m_dwDeleteCount;
};

DEPRECATED_BY(MemPool::MIN_UNIT_SIZE)
const size_t MIN_MEMUNIT_SIZE = MemPool::MIN_UNIT_SIZE;

DEPRECATED_BY(MemPool::MAX_UNIT_SIZE)
const size_t MAX_MEMUNIT_SIZE = MemPool::MAX_UNIT_SIZE;

DEPRECATED_BY(MemPool::MAX_INDEX_NUM)
const int MAX_INDEX_NUM = MemPool::MAX_INDEX;

//////////////////////////////////////////////////////////////////////////
// ��ʼ�� MemPool Ĭ��ʵ��
// ���أ��ڼ��α���ʼ��
// ע�⣺ֻ�е�һ��ʱ�������Ż��������õ���
int MemPool_Initialize(size_t dwMaxMemSize = MemPool::MAX_UNIT_SIZE);

//////////////////////////////////////////////////////////////////////////
// �ͷ� MemPool ʵ��
// ����: 0 �ͷųɹ�
//       >0 ����ǰ�Ķ�γ�ʼ����û��δ�ͷ�
int MemPool_Destroy();

//////////////////////////////////////////////////////////////////////////
// �̰߳�ȫ�ķ����ڴ���Դ
void* MemPool_Allocate(size_t dwSize);
void MemPool_Free(void* p);

size_t MemPool_GetAllocatedSize();
size_t MemPool_GetPooledSize();
size_t MemPool_GetAllocateCount();
size_t MemPool_GetFreeCount();
size_t MemPool_GetNewCount();
size_t MemPool_GetDeleteCount();

size_t MemPool_GetBlockSize(const void* ptr);

} // namespace common

#endif // COMMON_SYSTEM_MEMORY_MEMPOOL_H

