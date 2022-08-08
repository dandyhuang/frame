//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-12-09 16:41
//  @file:      mempool.cpp
//  @author:    
//  @brief:     
//
//********************************************************************

#include "thirdparty/monitor_sdk_common/system/memory/mempool.h"

#include <assert.h>
#include <stdio.h>
#include <algorithm>

#include "thirdparty/monitor_sdk_common/base/array_size.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/atomic.h"

namespace common {

const size_t MemPool::MIN_UNIT_SIZE;
const size_t MemPool::MAX_UNIT_SIZE;
const int MemPool::MAX_INDEX;

struct MemPool::BLOCK_HEADER
{
    unsigned char Magic[2]; // magic number to check, must be 'MP'
    short Index;            // �ڴ浥Ԫ������-1 ��ʾ���� cache ��
    unsigned int Size;      // ��Ԫ�Ĵ�С
};

struct BlockConfig
{
    size_t size;
    unsigned int cache_count;
    bool operator<(const BlockConfig& other) const
    {
        return size < other.size;
    }
};

static BlockConfig s_BlockConfig[] =
{
    { MemPool::MIN_UNIT_SIZE, 50 },
    { 2 * MemPool::MIN_UNIT_SIZE, 50 },
    { 4 * MemPool::MIN_UNIT_SIZE, 50 },
    { 8 * MemPool::MIN_UNIT_SIZE, 50 },
    { 16 * MemPool::MIN_UNIT_SIZE, 50 },
    { 32 * MemPool::MIN_UNIT_SIZE, 50 },
    { 64 * MemPool::MIN_UNIT_SIZE, 50 },
    { 128 * MemPool::MIN_UNIT_SIZE, 8 },
    { 256 * MemPool::MIN_UNIT_SIZE, 4 },
    { 512 * MemPool::MIN_UNIT_SIZE, 2 },
    { 1024 * MemPool::MIN_UNIT_SIZE, 1 },
    { 2048 * MemPool::MIN_UNIT_SIZE, 1 },
    { 4096 * MemPool::MIN_UNIT_SIZE, 1 },
    { 8192 * MemPool::MIN_UNIT_SIZE, 1 },
    { 16384 * MemPool::MIN_UNIT_SIZE, 1 },
    { 32768 * MemPool::MIN_UNIT_SIZE, 1 },
};

MemPool::MemPool(size_t dwMaxMemSize):
    m_poolMutex(),
    m_dwQueueNum(MAX_INDEX),
    m_allocated_memory_size(0),
    m_pooled_memory_size(0),
    m_dwAllocateCount(0),
    m_dwFreeCount(0),
    m_dwNewCount(0),
    m_dwDeleteCount(0)
{
    assert(dwMaxMemSize >= MIN_UNIT_SIZE);
    assert((dwMaxMemSize % MIN_UNIT_SIZE) == 0);
    assert(dwMaxMemSize <= MAX_UNIT_SIZE);
}

MemPool::~MemPool()
{
    //////////////////////////////////////////////////////////////////////////
    // �ͷ�MemPool�е��ڴ�;
    BLOCK_HEADER* pBlock = NULL;

    for (unsigned int i = 0; i < m_dwQueueNum; i++)
    {
        while (!(m_memPool[i].empty()))
        {
            pBlock = m_memPool[i].front();

            if (pBlock != NULL)
            {
                FreeBlock(pBlock);
            }

            m_memPool[i].pop_front();
        }
    }
}

bool MemPool::IsValidMemoryBlock(const MemPool::BLOCK_HEADER* block_header)
{
    if (block_header->Magic[0] != 'M' || block_header->Magic[1] != 'P')
        return false;

    if (block_header->Index == -1)
        return block_header->Size > MAX_UNIT_SIZE;

    return block_header->Index < MAX_INDEX &&
        block_header->Size <= MAX_UNIT_SIZE;
}

MemPool::BLOCK_HEADER* MemPool::AllocateBlock(unsigned int index)
{
    size_t size = sizeof(BLOCK_HEADER) + s_BlockConfig[index].size;
    BLOCK_HEADER* p = static_cast<BLOCK_HEADER*>(operator new(size));
    p->Magic[0] = 'M';
    p->Magic[1] = 'P';
    p->Index = index;
    p->Size = s_BlockConfig[index].size;
    AtomicIncrement(&m_dwNewCount);
    return p;
}

MemPool::BLOCK_HEADER* MemPool::AllocateLargeBlock(size_t size)
{
    size_t new_size = sizeof(BLOCK_HEADER) + size;
    BLOCK_HEADER* p = static_cast<BLOCK_HEADER*>(operator new(new_size));
    p->Magic[0] = 'M';
    p->Magic[1] = 'P';
    p->Index = -1;
    p->Size = size;
    AtomicIncrement(&m_dwNewCount);
    return p;
}

void MemPool::FreeBlock(BLOCK_HEADER* block)
{
    operator delete(block);
    AtomicIncrement(&m_dwDeleteCount);
}

//////////////////////////////////////////////////////////////////////////
// ��������:             ����һ����СΪdwSize��MemUnit
// �������:             @����1: dwSize; MemUnit�Ĵ�С;
// ����ֵ:               MemUnit��ָ��;
void* MemPool::Allocate(size_t dwSize)
{
    BLOCK_HEADER* pBlock = NULL;
    if (dwSize != 0)
    {
        int dwArrayIndex = GetUnitIndex(dwSize);

        if (dwArrayIndex >= MAX_INDEX)
        {
            pBlock = AllocateLargeBlock(dwSize);
        }
        else
        {
            {
                Spinlock::Locker locker(&m_poolMutex);

                if (!m_memPool[dwArrayIndex].empty())
                {
                    m_pooled_memory_size -= s_BlockConfig[dwArrayIndex].size;
                    pBlock = m_memPool[dwArrayIndex].front();
                    m_memPool[dwArrayIndex].pop_front();
                }
            }

            if (!pBlock)
            {
                pBlock = AllocateBlock(dwArrayIndex);
            }
        }
    }

    if (pBlock)
    {
        Spinlock::Locker locker(&m_poolMutex);
        ++m_dwAllocateCount;
        m_allocated_memory_size += pBlock->Size;
        return pBlock + 1;
    }

    return NULL;
}

// ��������:                �ͷ�һ��MemUnit����Դ
// �������:                @����1: pUnit, �ڴ���ָ��;
// ����ֵ:              ��
void MemPool::Free(void* p)
{
    if (p)
    {
        BLOCK_HEADER* pBlock = reinterpret_cast<BLOCK_HEADER*>(p) - 1;
        assert(IsValidMemoryBlock(pBlock));

        int dwArrayIndex = pBlock->Index;
        size_t size = pBlock->Size;

        if (dwArrayIndex == -1)
        {
            FreeBlock(pBlock);
        }
        else
        {
            {
                Spinlock::Locker locker(&m_poolMutex);
                size_t dwCurQueueSize = m_memPool[dwArrayIndex].size();
                if (dwCurQueueSize < s_BlockConfig[dwArrayIndex].cache_count)
                {
                    m_memPool[dwArrayIndex].push_back(pBlock);
                    m_pooled_memory_size += s_BlockConfig[dwArrayIndex].size;
                    pBlock = NULL;
                }
            }

            if (pBlock)
            {
                FreeBlock(pBlock);
            }
        }

        {
            Spinlock::Locker locker(&m_poolMutex);
            ++m_dwFreeCount;
            m_allocated_memory_size -= size;
        }
    }
}

size_t MemPool::GetBlockSize(const void* ptr) const
{
    if (ptr)
    {
        const BLOCK_HEADER* pBlock = reinterpret_cast<const BLOCK_HEADER*>(ptr) - 1;
        return pBlock->Size;
    }
    return 0;
}

size_t MemPool::GetAllocatedSize() const
{
    return m_allocated_memory_size;
}

size_t MemPool::GetPooledSize() const
{
    return m_pooled_memory_size;
}

size_t MemPool::GetAllocateCount() const
{
    return m_dwAllocateCount;
}

size_t MemPool::GetFreeCount() const
{
    return m_dwFreeCount;
}

size_t MemPool::GetNewCount() const
{
    return m_dwNewCount;
}

size_t MemPool::GetDeleteCount() const
{
    return m_dwDeleteCount;
}

// ��������:                ��ȡ�ڴ���ѭ��Buffer�������;
// �������:                @����1: dwSize, �ڴ��Ĵ�С;
// ����ֵ:              �������ֵ;
int MemPool::GetUnitIndex(size_t dwSize)
{
    BlockConfig block = { dwSize, 0 };
    BlockConfig* p = std::lower_bound(s_BlockConfig,
                                      s_BlockConfig + COMMON_ARRAY_SIZE(s_BlockConfig),
                                      block);
    return p - s_BlockConfig;
}

static int s_initialize_count;
static MemPool* s_pMemPool = NULL;

/// ��ʼ��MemPool
int MemPool_Initialize(size_t dwMaxMemSize)
{
    int count = AtomicIncrement(&s_initialize_count);
    if (count == 1)
    {
        if (s_pMemPool == NULL)
            s_pMemPool = new MemPool(dwMaxMemSize);
    }
    else // TODO(phongchen): wait for s_pMemPool initialize complete
    {
    }

    return count;
}

/// �ͷ�MemPool
int MemPool_Destroy()
{
    int count = AtomicDecrement(&s_initialize_count);
    if (count < 0)
    {
        fprintf(stderr, "Fatal error: unbalance Initialize/Destroy");
        abort();
    }

    if (count == 0)
    {
        delete s_pMemPool;
        s_pMemPool = NULL;
    }
    return count;
}

/// �̰߳�ȫ�ķ����ڴ���Դ
void* MemPool_Allocate(size_t dwSize)
{
    assert(s_pMemPool != NULL);
    return s_pMemPool->Allocate(dwSize);
}

void MemPool_Free(void* p)
{
    assert(s_pMemPool);
    s_pMemPool->Free(p);
}

size_t MemPool_GetAllocatedSize()
{
    assert(s_pMemPool);
    return s_pMemPool->GetAllocatedSize();
}

size_t MemPool_GetPooledSize()
{
    assert(s_pMemPool);
    return s_pMemPool->GetPooledSize();
}

size_t MemPool_GetAllocateCount()
{
    assert(s_pMemPool);
    return s_pMemPool->GetAllocateCount();
}

size_t MemPool_GetFreeCount()
{
    assert(s_pMemPool);
    return s_pMemPool->GetFreeCount();
}

size_t MemPool_GetNewCount()
{
    assert(s_pMemPool);
    return s_pMemPool->GetNewCount();
}

size_t MemPool_GetDeleteCount()
{
    assert(s_pMemPool);
    return s_pMemPool->GetDeleteCount();
}

size_t MemPool_GetBlockSize(const void* ptr)
{
    assert(s_pMemPool);
    return s_pMemPool->GetBlockSize(ptr);
}

} // namespace common

