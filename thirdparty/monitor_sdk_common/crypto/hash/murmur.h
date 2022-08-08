// Copyright (c) 2010, Vivo Inc.
// All rights reserved.
//

#ifndef COMMON_CRYPTO_HASH_MURMUR_H
#define COMMON_CRYPTO_HASH_MURMUR_H

#include <string.h>
#include <string>
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/crypto/hash/murmur/MurmurHash3.h"

// MurmurHash is a non-cryptographic hash function suitable for general
// hash-based lookup.
// It was created by Austin Appleby, and exists in a number of variants,
// all of which have been released into the public domain.
// see:
// http://en.wikipedia.org/wiki/MurmurHash
// http://sites.google.com/site/murmurhash/

namespace common {

uint32_t MurmurHash2(const void *data, size_t len, uint32_t seed);
uint32_t MurmurHash2A(const void * key, size_t len, uint32_t seed);
uint32_t MurmurHashAligned2(const void * key, size_t len, uint32_t seed);
uint32_t MurmurHashNeutral2(const void * key, size_t len, uint32_t seed);
uint64_t MurmurHash64A(const void * key, size_t len, uint64_t seed);
uint64_t MurmurHash64B(const void * key, size_t len, uint64_t seed);

struct Murmur2Hasher32
{
    typedef uint32_t ResultType;

    uint32_t operator()(const void *start, size_t len, uint32_t seed) const
    {
        return MurmurHash2(start, len, seed);
    }

    uint32_t operator()(const char *s, uint32_t seed) const
    {
        return MurmurHash2(s, strlen(s), seed);
    }
};

struct Murmur2Hasher64
{
    typedef uint64_t ResultType;

    uint64_t operator()(const void *start, size_t len, uint64_t seed) const
    {
        return MurmurHash64B(start, len, seed);
    }

    uint64_t operator()(const char *s, uint64_t seed) const
    {
        return MurmurHash64B(s, strlen(s), seed);
    }
};

//-----------------------------------------------------------------------------
// CMurmurHash2A, by Austin Appleby

// This is a sample implementation of MurmurHash2A designed to work
// incrementally.

// Usage -

// CMurmurHash2A hasher
// hasher.Begin(seed);
// hasher.Add(data1,size1);
// hasher.Add(data2,size2);
// ...
// hasher.Add(dataN,sizeN);
// uint32_t hash = hasher.End()

class CMurmurHash2A
{
public:
    explicit CMurmurHash2A(uint32_t seed = 0)
    {
        Begin(seed);
    }
    void Begin(uint32_t seed = 0);
    void Add(const void * data, size_t len);
    uint32_t End(void);
private:
    static const uint32_t m = 0x5bd1e995;
    static const int r = 24;
private:
    void MixTail(const unsigned char * & data, size_t & len);
private:
    uint32_t m_hash;
    uint32_t m_tail;
    uint32_t m_count;
    uint32_t m_size;
};

} // namespace common

#endif // COMMON_CRYPTO_HASH_MURMUR_H
