// Copyright (c) 2015, Tencent Inc. All rights reserved.
// Author:
// Description: BloomFilter implementation

#ifndef COMMON_COLLECTION_BLOOM_FILTER_H
#define COMMON_COLLECTION_BLOOM_FILTER_H

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <exception>
#include <stdexcept>
#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/base/string/format.h"
#include "thirdparty/monitor_sdk_common/base/uint32_divisor.h"
#include "thirdparty/monitor_sdk_common/crypto/hash/md4.h"
#include "thirdparty/monitor_sdk_common/crypto/hash/murmur.h"
#include "thirdparty/monitor_sdk_common/crypto/hash/murmur/MurmurHash3.h"

// GLOBAL_NOLINT(runtime/int)

namespace common {

#if defined __i386__ || defined __x86_64__
/**
 * __test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
inline bool x86_test_and_set_bit(int nr, volatile unsigned long * addr)
{
    bool oldbit;

    __asm__(
        "btsl %2,%1\n\tsetc %0"
        :"=q" (oldbit), "+m" (*addr)
        :"Ir" (nr));
    return oldbit;
}
#endif

/**
 * A space-efficent probabilistic set for membership test, false postives
 * are possible, but false negatives are not.
 */
template <class Hasher = Murmur2Hasher32>
class BasicBloomFilter
{
public:
    /// default ctor, set bloom filter to uninitialized state
    BasicBloomFilter()
    {
        InitialClear();
    }

    /// @param element_count max optimized element count
    /// @param false_positive_prob false positive prob when reach max element count
    BasicBloomFilter(size_t element_count, double false_positive_prob)
    {
        InitialClear();
        Initialize(element_count, false_positive_prob);
    }

    /// @param bitmap_byte_size bitmap byte size
    /// @param num_hashes number of hash functions
    BasicBloomFilter(size_t bitmap_byte_size, size_t num_hashes)
    {
        InitialClear();
        Initialize(bitmap_byte_size, num_hashes);
    }

    /// @param bitmap extsited bitmap
    /// @param bitmap_byte_size bitmap byte size
    /// @param num_hashes number of hash functions
    /// @param copy whether copy bitmap
    BasicBloomFilter(void* bitmap, size_t bitmap_byte_size, size_t num_hashes, bool copy = true)
    {
        InitialClear();
        Initialize(bitmap, bitmap_byte_size, num_hashes, copy);
    }

    virtual ~BasicBloomFilter()
    {
        Destroy();
    }

    /// @param bitmap extsited bitmap
    /// @param bitmap_byte_size bitmap byte size
    /// @param num_hashes number of hash functions
    /// @param copy whether copy bitmap
    void Initialize(void* bitmap, size_t bitmap_byte_size, int num_hashes, bool copy = true)
    {
        Destroy();
        CheckBitmapSize(bitmap_byte_size);
        if (copy)
        {
            void* bitmap_copy = calloc(bitmap_byte_size, 1);
            if (!bitmap_copy)
                throw std::bad_alloc();
            memcpy(bitmap_copy, bitmap, bitmap_byte_size);
            bitmap = bitmap_copy;
        }
        UncheckedInitialize(bitmap, bitmap_byte_size, num_hashes);
        m_own_bitmap = copy;
    }

    /// @param bitmap_byte_size bitmap byte size
    /// @param num_hashes number of hash functions
    void Initialize(size_t bitmap_byte_size, size_t num_hashes)
    {
        Destroy();
        CheckBitmapSize(bitmap_byte_size);
        UncheckedInitialize(bitmap_byte_size, num_hashes);
    }

    /// @param element_count max optimized element count
    /// @param false_positive_prob false positive prob when reach max element count
    void Initialize(size_t element_count, double false_positive_prob)
    {
        double num_hashes = -log(false_positive_prob) / log(2.0);
        size_t num_hash_functions = (size_t) ceil(num_hashes + 0.001);
        unsigned long long num_bits =
            (unsigned long long)(element_count * num_hash_functions / log(2.0));

        // round up
        unsigned long long num_bytes = (num_bits + CHAR_BIT - 1) / CHAR_BIT;
        num_bits = num_bytes * CHAR_BIT; // then in CHAR_BIT times

        if (num_bits == 0)
        {
            throw std::runtime_error(StringFormat("Num elements=%lu false_positive_prob=%g",
                    (unsigned long)element_count, false_positive_prob));
        }

        if (num_bytes > ~size_t(0))
        {
            throw std::runtime_error(
                StringFormat("Bitmap too large, size=%llu, exceed size_t limitation", num_bytes));
        }

        Initialize((size_t) num_bytes, num_hash_functions);

        // m_element_count = element_count;
        // m_false_positive_prob = false_positive_prob;
    }

    /// destroy the bloom filter and free all allocated resources
    void Destroy()
    {
        if (m_own_bitmap)
        {
            free(m_bitmap);
        }
        InitialClear();
    }

    /// insert a key
    void Insert(const void *key, size_t len)
    {
        uint32_t digest[4];
        for (size_t i = 0; i < m_num_hash_functions; ++i)
        {
            if (i % 4 == 0)
            {
                if (i != 0) // use original key at first time
                    MurmurHash3_x86_128(digest, 16, i, digest);
                else
                    MurmurHash3_x86_128(key, len, 0, digest);
            }

            uint32_t bit_index = m_divisor.Modulu(digest[i % 4]);
            assert(bit_index == digest[i % 4] % m_num_bits);
            m_bitmap[bit_index / CHAR_BIT] |= (1 << (bit_index % CHAR_BIT));
        }
    }

    /// insert a c string
    void Insert(const char* key)
    {
        Insert(key, strlen(key));
    }

    /// insert a c++ string
    void Insert(const std::string& key)
    {
        Insert(key.data(), key.size());
    }

    /// try insert an unique key and return previous status
    /// @retval true key doesn't exist before insert
    /// @retval false key exist or false positive (conflict) before insert
    bool InsertUnique(const void *key, size_t len)
    {
        unsigned int exist_count = 0;
        uint32_t digest[4];
        for (size_t i = 0; i < m_num_hash_functions; ++i)
        {
            if (i % 4 == 0)
            {
                if (i != 0)
                    MurmurHash3_x86_128(digest, 16, i, digest);
                else // use original key at first time
                    MurmurHash3_x86_128(key, len, 0, digest);
            }

            uint32_t bit_index = m_divisor.Modulu(digest[i % 4]);
            assert(bit_index == digest[i % 4] % m_num_bits);
#if defined __i386__ || defined __x86_64__
            exist_count += x86_test_and_set_bit(
                bit_index,
                reinterpret_cast<unsigned long*>(m_bitmap)
            );
#else
            uint8_t mask = (1 << (bit_index % CHAR_BIT));
            exist_count += (m_bitmap[bit_index / CHAR_BIT] & mask) != 0;
            m_bitmap[bit_index / CHAR_BIT] |= mask;
#endif
        }
        return exist_count < m_num_hash_functions;
    }

    bool InsertUnique(const char* key)
    {
        return InsertUnique(key, strlen(key));
    }

    bool InsertUnique(const std::string& key)
    {
        return InsertUnique(key.data(), key.size());
    }

    /// clear all keys
    void Clear()
    {
        memset(m_bitmap, 0, MemorySize());
    }

    /// @return possible existance of key
    bool MayContain(const void *key, size_t len) const
    {
        uint32_t digest[4];
        for (size_t i = 0; i < m_num_hash_functions; ++i)
        {
            if (i % 4 == 0)
            {
                if (i != 0) // use original key at first time
                    MurmurHash3_x86_128(digest, 16, i, digest);
                else
                    MurmurHash3_x86_128(key, len, 0, digest);
            }

            uint32_t bit_index = m_divisor.Modulu(digest[i % 4]);
            assert(bit_index == digest[i % 4] % m_num_bits);
            uint8_t byte = m_bitmap[bit_index / CHAR_BIT];
            uint8_t mask = (1 << (bit_index % CHAR_BIT));

            if ( (byte & mask) == 0 )
            {
                return false;
            }
        }
        return true;
    }

    /// @return possible existance of key
    bool MayContain(const std::string& key) const
    {
        return MayContain(key.data(), key.size());
    }

    /// @return possible existance of key
    bool MayContain(const char* key) const
    {
        return MayContain(key, strlen(key));
    }

    /// is correct initialized
    bool IsValid() const
    {
        return m_bitmap != NULL;
    }

    /// total bit count
    unsigned long long TotalBits() const
    {
        assert(IsValid());
        return m_num_bits;
    }

    /// total memory used, in bytes
    size_t MemorySize() const
    {
        assert(IsValid());
        return (size_t)((m_num_bits + CHAR_BIT - 1) / CHAR_BIT);
    }

    /// @return number of hash functions
    unsigned int HashNumber() const
    {
        assert(IsValid());
        return m_num_hash_functions;
    }

    bool IsOwnBitmap()
    {
        return m_own_bitmap;
    }

    unsigned char* GetBitmap()
    {
        return m_bitmap;
    }
    const unsigned char* GetBitmap() const
    {
        return m_bitmap;
    }

    Hasher GetHasher() const
    {
        return m_hasher;
    }

    unsigned long long Capacity() const
    {
        assert(IsValid());
        return m_element_count;
    }

    double FalsePositiveProb() const
    {
        assert(IsValid());
        return m_false_positive_prob;
    }

private:
    /// helper function used by ctors to initialize all members
    void InitialClear()
    {
        m_element_count = 0;
        m_false_positive_prob = 1.0;
        m_num_hash_functions = 0;
        m_bitmap = NULL;
        m_num_bits = 0;
        m_own_bitmap = false;
    }

    void UncheckedInitialize(
        void* bitmap,
        size_t bitmap_byte_size,
        size_t num_hashes
        )
    {
        m_num_hash_functions = num_hashes;
        m_bitmap = reinterpret_cast<uint8_t*>(bitmap);
        unsigned long long num_bits = (unsigned long long) bitmap_byte_size * CHAR_BIT;
        m_num_bits = num_bits;
        if (!m_divisor.SetValue(m_num_bits))
            abort();
        m_false_positive_prob = exp(-log(2.0) * num_hashes);
        m_element_count = static_cast<size_t>(
            num_bits * log(2.0) / m_num_hash_functions
        );
    }

    void UncheckedInitialize(size_t bitmap_byte_size, size_t num_hashes)
    {
        void* bitmap = calloc(bitmap_byte_size, 1);
        if (!bitmap)
            throw std::bad_alloc();
        UncheckedInitialize(bitmap, bitmap_byte_size, num_hashes);
        m_own_bitmap = true;
    }

    static void CheckBitmapSize(size_t byte_size)
    {
        unsigned long long num_bits = (unsigned long long) byte_size * CHAR_BIT;
        if (num_bits >= ~typename Hasher::ResultType(0))
        {
            throw std::runtime_error(StringFormat(
                "Bitmap too large, bit count=%llu, exceed 4Gbits(512 Mbytes)",
                num_bits));
        }
    }

private:
    BasicBloomFilter(const BasicBloomFilter&);
    BasicBloomFilter& operator=(const BasicBloomFilter&);

private:
    Hasher     m_hasher;
    size_t     m_element_count;
    double     m_false_positive_prob;
    size_t     m_num_hash_functions;
    uint8_t*   m_bitmap;
    unsigned long long m_num_bits;
    UInt32Divisor m_divisor;
    bool m_own_bitmap; ///< whether we own the bitmap
};

template <class Hasher = Murmur2Hasher32>
class BasicPartialBloomFilter
{
public:
    BasicPartialBloomFilter(size_t element_count, double false_positive_prob)
    {
        Initialize(element_count, false_positive_prob);
    }

    ~BasicPartialBloomFilter()
    {
        for (size_t i = 0; i < m_bitmaps.size(); ++i)
            free(m_bitmaps[i]);
    }

    /* XXX/review static functions to expose the bloom filter parameters, given
       1) probablility and # keys
       2) #keys and fix the total size (m), to calculate the optimal
       probability - # hash functions to use
       */

    void Insert(const void *key, size_t len)
    {
        typename Hasher::ResultType hash = 0;

        for (size_t i = 0; i < m_num_hash_functions; ++i)
        {
            hash = m_hasher(key, len, hash);
            typename Hasher::ResultType bit_index = hash % m_num_bits;
            m_bitmaps[i][bit_index / CHAR_BIT] |= (1 << (bit_index % CHAR_BIT));
        }
    }

    void Insert(const char* key)
    {
        Insert(key, strlen(key));
    }

    void Insert(const std::string& key)
    {
        Insert(key.data(), key.size());
    }

    bool MayContain(const void *key, size_t len) const
    {
        typename Hasher::ResultType hash = 0;

        for (size_t i = 0; i < m_num_hash_functions; ++i)
        {
            hash = m_hasher(key, len, hash);

            typename Hasher::ResultType bit_index = hash % m_num_bits;
            uint8_t byte = m_bitmaps[i][bit_index / CHAR_BIT];
            uint8_t byte_mask = uint8_t(1 << (bit_index % CHAR_BIT));

            if ( (byte & byte_mask) == 0 )
            {
                return false;
            }
        }
        return true;
    }

    bool MayContain(const std::string& key) const
    {
        return MayContain(key.data(), key.size());
    }

    bool MayContain(const char* key) const
    {
        return MayContain(key, strlen(key));
    }

    size_t MemorySize() const
    {
        return m_num_hash_functions * (m_num_bits + CHAR_BIT - 1) / CHAR_BIT;
    }

    unsigned int HashNumber() const
    {
        return m_num_hash_functions;
    }
private:
    void Initialize(size_t element_count, double false_positive_prob)
    {
        m_element_count = element_count;
        m_false_positive_prob = false_positive_prob;
        double num_hashes = -log(m_false_positive_prob) / log(2);
        m_num_hash_functions = (size_t)(num_hashes + 0.5);
        unsigned long long total_bits =
            (unsigned long long)(m_element_count * num_hashes / log(2));
        if (total_bits == 0)
        {
            throw std::runtime_error(StringFormat("Num elements=%lu false_positive_prob=%g",
                                                  (unsigned long)element_count,
                                                  false_positive_prob));
        }
        unsigned long long single_bits =
            (unsigned long long)(total_bits / num_hashes);

        if (single_bits >= ~typename Hasher::ResultType(0))
        {
            throw std::runtime_error(
                StringFormat("Bitmap too large, bit count=%llu, exceed 4Gbits(512 Mbytes)",
                             single_bits));
        }

        m_num_bits = single_bits;
        m_bitmaps.resize(m_num_hash_functions);

        unsigned long long num_bytes = (single_bits + CHAR_BIT - 1) / CHAR_BIT;
        if (num_bytes > ~size_t(0))
        {
            throw std::runtime_error(
                StringFormat("Bitmap too large, size=%llu, exceed size_t limitation",
                             num_bytes
                )
            );
        }

        for (size_t i = 0; i < m_num_hash_functions; ++i)
        {
            m_bitmaps[i] = static_cast<uint8_t*>(calloc(num_bytes, 1));
            if (!m_bitmaps[i])
            {
                for (size_t j = 0; j < i; ++j)
                     free(m_bitmaps[j]);
                throw std::bad_alloc();
            }
        }
    }

private:
    Hasher     m_hasher;
    size_t     m_element_count;
    double     m_false_positive_prob;
    size_t     m_num_hash_functions;
    unsigned long long m_num_bits;
    std::vector<uint8_t*>   m_bitmaps;
};

typedef BasicBloomFilter<> BloomFilter;
typedef BasicPartialBloomFilter<> PartialBloomFilter;

} // namespace common


#endif // COMMON_COLLECTION_BLOOM_FILTER_H
