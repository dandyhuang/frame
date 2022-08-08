// Copyright (c) 2011, Vivo Inc.
// All rights reserved.
//
// Created: 04/14/2011 02:11:09 PM
// Description: from http://cityhash.googlecode.com/svn/trunk/src/city.cc

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

#include <algorithm>
#include <utility>

#include "thirdparty/monitor_sdk_common/base/compatible/unistd.h"
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/crypto/hash/city.h"
#include "thirdparty/monitor_sdk_common/system/memory/unaligned.h"

using namespace std;


namespace common {

#if !defined(LIKELY)
#if defined(__GNUC__)
#define LIKELY(x) (__builtin_expect(!!(x), 1))
#else
#define LIKELY(x) (x)
#endif
#endif

// Some primes between 2^63 and 2^64 for various uses.
static const uint64_t k0 = 0xc3a5c85c97cb3127ULL;
static const uint64_t k1 = 0xb492b66fbe98f273ULL;
static const uint64_t k2 = 0x9ae16a3b2f90404fULL;
static const uint64_t k3 = 0xc949d7c7509e6557ULL;

// Bitwise right rotate.  Normally this will compile to a single
// instruction, especially if the shift is a manifest constant.
static uint64_t Rotate(uint64_t val, int shift) {
    // Avoid shifting by 64: doing so yields an undefined result.
    return shift == 0 ? val : ((val >> shift) | (val << (64 - shift)));
}

// Equivalent to Rotate(), but requires the second arg to be non-zero.
// On x86-64, and probably others, it's possible for this to compile
// to a single instruction if both args are already in registers.
static uint64_t RotateByAtLeast1(uint64_t val, int shift) {
    return (val >> shift) | (val << (64 - shift));
}

static uint64_t ShiftMix(uint64_t val) {
    return val ^ (val >> 47);
}

static uint64_t HashLen16(uint64_t u, uint64_t v) {
    return Hash128to64(UInt128(u, v));
}

static uint64_t HashLen0to16(const char *buf, size_t len) {
    const char* s = static_cast<const char*>(buf);
    if (len > 8) {
        uint64_t a = GetUnaligned<uint64_t>(s);
        uint64_t b = GetUnaligned<uint64_t>(s + len - 8);
        return HashLen16(a, RotateByAtLeast1(b + len, len)) ^ b;
    }
    if (len >= 4) {
        uint64_t a = GetUnaligned<uint32_t>(s);
        return HashLen16(len + (a << 3), GetUnaligned<uint32_t>(s + len - 4));
    }
    if (len > 0) {
        uint8_t a = s[0];
        uint8_t b = s[len >> 1];
        uint8_t c = s[len - 1];
        uint32_t y = static_cast<uint32_t>(a) + (static_cast<uint32_t>(b) << 8);
        uint32_t z = len + (static_cast<uint32_t>(c) << 2);
        return ShiftMix(y * k2 ^ z * k3) * k2;
    }
    return k2;
}

// This probably works well for 16-byte strings as well, but it may be overkill
// in that case.
static uint64_t HashLen17to32(const char *buf, size_t len) {
    const char* s = static_cast<const char*>(buf);
    uint64_t a = GetUnaligned<uint64_t>(s) * k1;
    uint64_t b = GetUnaligned<uint64_t>(s + 8);
    uint64_t c = GetUnaligned<uint64_t>(s + len - 8) * k2;
    uint64_t d = GetUnaligned<uint64_t>(s + len - 16) * k0;
    return HashLen16(Rotate(a - b, 43) + Rotate(c, 30) + d,
                     a + Rotate(b ^ k3, 20) - c + len);
}

// Return a 16-byte hash for 48 bytes.  Quick and dirty.
// Callers do best to use "random-looking" values for a and b.
static pair<uint64_t, uint64_t> WeakHashLen32WithSeeds(
    uint64_t w, uint64_t x, uint64_t y, uint64_t z, uint64_t a, uint64_t b) {
    a += w;
    b = Rotate(b + a + z, 21);
    uint64_t c = a;
    a += x;
    a += y;
    b += Rotate(a, 44);
    return make_pair(a + z, b + c);
}

// Return a 16-byte hash for s[0] ... s[31], a, and b.  Quick and dirty.
static pair<uint64_t, uint64_t> WeakHashLen32WithSeeds(
    const char* buf, uint64_t a, uint64_t b)
{
    const char* s = static_cast<const char*>(buf);
    return WeakHashLen32WithSeeds(GetUnaligned<uint64_t>(s),
                                  GetUnaligned<uint64_t>(s + 8),
                                  GetUnaligned<uint64_t>(s + 16),
                                  GetUnaligned<uint64_t>(s + 24),
                                  a,
                                  b);
}

// Return an 8-byte hash for 33 to 64 bytes.
static uint64_t HashLen33to64(const char *buf, size_t len) {
    const char* s = static_cast<const char*>(buf);
    uint64_t z = GetUnaligned<uint64_t>(s + 24);
    uint64_t a = GetUnaligned<uint64_t>(s) + (len + GetUnaligned<uint64_t>(s + len - 16)) * k0;
    uint64_t b = Rotate(a + z, 52);
    uint64_t c = Rotate(a, 37);
    a += GetUnaligned<uint64_t>(s + 8);
    c += Rotate(a, 7);
    a += GetUnaligned<uint64_t>(s + 16);
    uint64_t vf = a + z;
    uint64_t vs = b + Rotate(a, 31) + c;
    a = GetUnaligned<uint64_t>(s + 16) + GetUnaligned<uint64_t>(s + len - 32);
    z = GetUnaligned<uint64_t>(s + len - 8);
    b = Rotate(a + z, 52);
    c = Rotate(a, 37);
    a += GetUnaligned<uint64_t>(s + len - 24);
    c += Rotate(a, 7);
    a += GetUnaligned<uint64_t>(s + len - 16);
    uint64_t wf = a + z;
    uint64_t ws = b + Rotate(a, 31) + c;
    uint64_t r = ShiftMix((vf + ws) * k2 + (wf + vs) * k0);
    return ShiftMix(r * k0 + vs) * k2;
}

uint64_t CityHash64(const void *buf, size_t len) {
    const char* s = static_cast<const char*>(buf);
    if (len <= 32) {
        if (len <= 16) {
            return HashLen0to16(s, len);
        } else {
            return HashLen17to32(s, len);
        }
    } else if (len <= 64) {
        return HashLen33to64(s, len);
    }

    // For strings over 64 bytes we hash the end first, and then as we
    // loop we keep 56 bytes of state: v, w, x, y, and z.
    uint64_t x = GetUnaligned<uint64_t>(s);
    uint64_t y = GetUnaligned<uint64_t>(s + len - 16) ^ k1;
    uint64_t z = GetUnaligned<uint64_t>(s + len - 56) ^ k0;
    pair<uint64_t, uint64_t> v = WeakHashLen32WithSeeds(s + len - 64, len, y);
    pair<uint64_t, uint64_t> w = WeakHashLen32WithSeeds(s + len - 32, len * k1, k0);
    z += ShiftMix(v.second) * k1;
    x = Rotate(z + x, 39) * k1;
    y = Rotate(y, 33) * k1;

    // Decrease len to the nearest multiple of 64, and operate on 64-byte chunks.
    len = (len - 1) & ~static_cast<size_t>(63);
    do {
        x = Rotate(x + y + v.first + GetUnaligned<uint64_t>(s + 16), 37) * k1;
        y = Rotate(y + v.second + GetUnaligned<uint64_t>(s + 48), 42) * k1;
        x ^= w.second;
        y ^= v.first;
        z = Rotate(z ^ w.first, 33);
        v = WeakHashLen32WithSeeds(s, v.second * k1, x + w.first);
        w = WeakHashLen32WithSeeds(s + 32, z + w.second, y);
        std::swap(z, x);
        s += 64;
        len -= 64;
    } while (len != 0);
    return HashLen16(HashLen16(v.first, w.first) + ShiftMix(y) * k1 + z,
                     HashLen16(v.second, w.second) + x);
}

uint64_t CityHash64WithSeed(const void *buf, size_t len, uint64_t seed) {
    return CityHash64WithSeeds(buf, len, k2, seed);
}

uint64_t CityHash64WithSeeds(const void *buf, size_t len,
                             uint64_t seed0, uint64_t seed1) {
    return HashLen16(CityHash64(buf, len) - seed0, seed1);
}

// A subroutine for CityHash128().  Returns a decent 128-bit hash for strings
// of any length representable in ssize_t.  Based on City and Murmur.
static UInt128 CityMurmur(const char *buf, size_t len, UInt128 seed) {
    const char* s = static_cast<const char*>(buf);
    uint64_t a = seed.low;
    uint64_t b = seed.high;
    uint64_t c = 0;
    uint64_t d = 0;
    ssize_t l = len - 16;
    if (l <= 0) {  // len <= 16
        c = b * k1 + HashLen0to16(s, len);
        d = Rotate(a + (len >= 8 ? GetUnaligned<uint64_t>(s) : c), 32);
    } else {  // len > 16
        c = HashLen16(GetUnaligned<uint64_t>(s + len - 8) + k1, a);
        d = HashLen16(b + len, c + GetUnaligned<uint64_t>(s + len - 16));
        a += d;
        do {
            a ^= ShiftMix(GetUnaligned<uint64_t>(s) * k1) * k1;
            a *= k1;
            b ^= a;
            c ^= ShiftMix(GetUnaligned<uint64_t>(s + 8) * k1) * k1;
            c *= k1;
            d ^= c;
            s += 16;
            l -= 16;
        } while (l > 0);
    }
    a = HashLen16(a, c);
    b = HashLen16(d, b);
    return UInt128(a ^ b, HashLen16(b, a));
}

UInt128 CityHash128WithSeed(const void *buf, size_t len, const UInt128& seed) {
    const char* s = static_cast<const char*>(buf);
    if (len < 128) {
        return CityMurmur(s, len, seed);
    }

    // We expect len >= 128 to be the common case.  Keep 56 bytes of state:
    // v, w, x, y, and z.
    pair<uint64_t, uint64_t> v, w;
    uint64_t x = seed.low;
    uint64_t y = seed.high;
    uint64_t z = len * k1;
    v.first = Rotate(y ^ k1, 49) * k1 + GetUnaligned<uint64_t>(s);
    v.second = Rotate(v.first, 42) * k1 + GetUnaligned<uint64_t>(s + 8);
    w.first = Rotate(y + z, 35) * k1 + x;
    w.second = Rotate(x + GetUnaligned<uint64_t>(s + 88), 53) * k1;

    // This is the same inner loop as CityHash64(), manually unrolled.
    do {
        x = Rotate(x + y + v.first + GetUnaligned<uint64_t>(s + 16), 37) * k1;
        y = Rotate(y + v.second + GetUnaligned<uint64_t>(s + 48), 42) * k1;
        x ^= w.second;
        y ^= v.first;
        z = Rotate(z ^ w.first, 33);
        v = WeakHashLen32WithSeeds(s, v.second * k1, x + w.first);
        w = WeakHashLen32WithSeeds(s + 32, z + w.second, y);
        std::swap(z, x);
        s += 64;
        x = Rotate(x + y + v.first + GetUnaligned<uint64_t>(s + 16), 37) * k1;
        y = Rotate(y + v.second + GetUnaligned<uint64_t>(s + 48), 42) * k1;
        x ^= w.second;
        y ^= v.first;
        z = Rotate(z ^ w.first, 33);
        v = WeakHashLen32WithSeeds(s, v.second * k1, x + w.first);
        w = WeakHashLen32WithSeeds(s + 32, z + w.second, y);
        std::swap(z, x);
        s += 64;
        len -= 128;
    } while (LIKELY(len >= 128));
    y += Rotate(w.first, 37) * k0 + z;
    x += Rotate(v.first + z, 49) * k0;
    // If 0 < len < 128, hash up to 4 chunks of 32 bytes each from the end of s.
    for (size_t tail_done = 0; tail_done < len; ) {
        tail_done += 32;
        y = Rotate(y - x, 42) * k0 + v.second;
        w.first += GetUnaligned<uint64_t>(s + len - tail_done + 16);
        x = Rotate(x, 49) * k0 + w.first;
        w.first += v.first;
        v = WeakHashLen32WithSeeds(s + len - tail_done, v.first, v.second);
    }
    // At this point our 48 bytes of state should contain more than
    // enough information for a strong 128-bit hash.  We use two
    // different 48-byte-to-8-byte hashes to get a 16-byte final result.
    x = HashLen16(x, v.first);
    y = HashLen16(y, w.first);
    return UInt128(HashLen16(x + v.second, w.second) + y,
                   HashLen16(x + w.second, y + v.second));
}

UInt128 CityHash128(const void *buf, size_t len) {
    const char* s = static_cast<const char*>(buf);
    if (len >= 16) {
        return CityHash128WithSeed(s + 16,
                                   len - 16,
                                   UInt128(GetUnaligned<uint64_t>(s) ^ k3,
                                           GetUnaligned<uint64_t>(s + 8)));
    } else if (len >= 8) {
        return CityHash128WithSeed(NULL,
                                   0,
                                   UInt128(GetUnaligned<uint64_t>(s) ^ (len * k0),
                                           GetUnaligned<uint64_t>(s + len - 8) ^ k1));
    } else {
        return CityHash128WithSeed(s, len, UInt128(k0, k1));
    }
}

} // namespace common

