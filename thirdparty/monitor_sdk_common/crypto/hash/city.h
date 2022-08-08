// Copyright (c) 2011, Vivo Inc.
// All rights reserved.
//
// Created: 04/14/2011 01:59:40 PM
// Description: from http://cityhash.googlecode.com/svn/trunk/src/city.h

// Copyright (c) 2011 Google, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// CityHash Version 1, by Geoff Pike and Jyrki Alakuijala
//
// This file provides CityHash64() and related functions.
//
// It's probably possible to create even faster hash functions by
// writing a program that systematically explores some of the space of
// possible hash functions, by using SIMD instructions, or by
// compromising on hash quality.

#ifndef COMMON_CRYPTO_HASH_CITY_H
#define COMMON_CRYPTO_HASH_CITY_H

#include <stdlib.h>  // for size_t.
#include "thirdparty/monitor_sdk_common/base/byte_order.h"
#include "thirdparty/monitor_sdk_common/base/stdint.h"

namespace common {

struct UInt128 {
#if BYTE_ORDER == LITTLE_ENDIAN
    uint64_t low;
    uint64_t high;
#else
    uint64_t low;
    uint64_t high;
#endif
    UInt128(uint64_t lo, uint64_t hi) : low(lo), high(hi) {
    }
};

// Hash function for a byte array.
uint64_t CityHash64(const void *buf, size_t len);

// Hash function for a byte array.  For convenience, a 64-bit seed is also
// hashed into the result.
uint64_t CityHash64WithSeed(const void *buf, size_t len, uint64_t seed);

// Hash function for a byte array.  For convenience, two seeds are also
// hashed into the result.
uint64_t CityHash64WithSeeds(const void *buf, size_t len,
                             uint64_t seed0, uint64_t seed1);

// Hash function for a byte array.
UInt128 CityHash128(const void *buf, size_t len);

// Hash function for a byte array.  For convenience, a 128-bit seed is also
// hashed into the result.
UInt128 CityHash128WithSeed(const void *buf, size_t len, const UInt128& seed);

// Hash 128 input bits down to 64 bits of output.
// This is intended to be a reasonably good hash function.
inline uint64_t Hash128to64(const UInt128& x) {
    // Murmur-inspired hashing.
    const uint64_t kMul = 0x9ddfea08eb382d69ULL;
    uint64_t a = x.low ^ x.high * kMul;
    a ^= (a >> 47);
    uint64_t b = (x.high ^ a) * kMul;
    b ^= (b >> 47);
    b *= kMul;
    return b;
}

} // namespace common

#endif // COMMON_CRYPTO_HASH_CITY_H
