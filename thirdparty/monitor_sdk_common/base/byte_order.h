// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Description: Byte order detect and convert

#ifndef COMMON_BASE_BYTE_ORDER_H
#define COMMON_BASE_BYTE_ORDER_H

// GLOBAL_NOLINT(runtime/int)

#include <stddef.h>

#ifdef __unix__
#include <byteswap.h>
#include <endian.h>
#include <arpa/inet.h>
#endif

#ifdef _MSC_VER
#include <stdlib.h> // for _byteswap_*
#endif

// define __LITTLE_ENDIAN
#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#endif

// define __BIG_ENDIAN
#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN 4321
#endif

// define __BYTE_ORDER
#ifndef __BYTE_ORDER
#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)
#define __BYTE_ORDER __LITTLE_ENDIAN
#else
#error unknown byte order
#endif // ARCH
#endif // __BYTE_ORDER

#define COMMON_LITTLE_ENDIAN __LITTLE_ENDIAN
#define COMMON_BIG_ENDIAN __BIG_ENDIAN
#define COMMON_BYTE_ORDER __BYTE_ORDER

/// define LITTLE_ENDIAN
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN __LITTLE_ENDIAN
#endif

/// define BIG_ENDIAN
#ifndef BIG_ENDIAN
#define BIG_ENDIAN __BIG_ENDIAN
#endif

/// define BYTE_ORDER
#ifndef BYTE_ORDER
#define BYTE_ORDER __BYTE_ORDER
#endif

#ifdef _WIN32 // winsock APIs

#define BYTEORDER_WINSOCK_API_LINKAGE __declspec(dllimport)
#define BYTEORDER_WSAAPI __stdcall

extern "C" {

BYTEORDER_WINSOCK_API_LINKAGE
unsigned long
BYTEORDER_WSAAPI
htonl(
     unsigned long hostlong
);

BYTEORDER_WINSOCK_API_LINKAGE
unsigned short
BYTEORDER_WSAAPI
htons(
     unsigned short hostshort
);

BYTEORDER_WINSOCK_API_LINKAGE
unsigned long
BYTEORDER_WSAAPI
ntohl(
     unsigned long netlong
);

BYTEORDER_WINSOCK_API_LINKAGE
unsigned short
BYTEORDER_WSAAPI
ntohs(
     unsigned short netshort
);

} // extern "C"

#endif // _WIN32

#ifndef __linux__
# ifdef _MSC_VER
static unsigned short bswap_16(unsigned short x) // NOLINT(runtime/int)
{
    return _byteswap_ushort(x);
}
static unsigned int bswap_32(unsigned int x) // NOLINT(runtime/int)
{
    return _byteswap_ulong(x);
}
static unsigned long long bswap_64(unsigned long long x) // NOLINT(runtime/int)
{
    return _byteswap_uint64(x);
}
# else
static unsigned short bswap_16(unsigned short x) // NOLINT(runtime/int)
{
    return (((x >> 8) & 0xff) | ((x & 0xff) << 8));
}
static unsigned int bswap_32(unsigned int x) // NOLINT(runtime/int)
{
    return
118         (((x & 0xff000000) >> 24) | ((x & 0x00ff0000) >>  8) |
119         ((x & 0x0000ff00) <<  8) | ((x & 0x000000ff) << 24));
120 }
static unsigned long long bswap_64(unsigned long long x) // NOLINT(runtime/int)
{
    return
        (((x & 0xff00000000000000ull) >> 56)
        | ((x & 0x00ff000000000000ull) >> 40)
        | ((x & 0x0000ff0000000000ull) >> 24)
        | ((x & 0x000000ff00000000ull) >> 8)
        | ((x & 0x00000000ff000000ull) << 8)
        | ((x & 0x0000000000ff0000ull) << 24)
        | ((x & 0x000000000000ff00ull) << 40)
        | ((x & 0x00000000000000ffull) << 56));
}
# endif
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
inline unsigned long long htonll(unsigned long long n) // NOLINT(runtime/int)
{
    return bswap_64(n);
}
#else
inline unsigned long long htonll(unsigned long long n) // NOLINT(runtime/int)
{
    return n;
}
#endif

inline unsigned long long ntohll(unsigned long long n) // NOLINT(runtime/int)
{
    return htonll(n);
}

namespace common {
namespace internal {

template <size_t Size> struct ByteOrderSwapperBase {};

template <>
struct ByteOrderSwapperBase<1> {
    template <typename T> static T Swap(T value) { return value; }
    template <typename T> static T hton(T value) { return value; }
    template <typename T> static T ntoh(T value) { return value; }
};

template <>
struct ByteOrderSwapperBase<2> {
    template <typename T> static T Swap(T value) { return bswap_16(value); }
    template <typename T> static T hton(T value) { return htons(value); }
    template <typename T> static T ntoh(T value) { return ntohs(value); }
};

template <>
struct ByteOrderSwapperBase<4> {
    template <typename T> static T Swap(T value) { return bswap_32(value); }
    template <typename T> static T hton(T value) { return htonl(value); }
    template <typename T> static T ntoh(T value) { return ntohl(value); }
};

template <>
struct ByteOrderSwapperBase<8> {
    template <typename T> static T Swap(T value) { return bswap_64(value); }
#if COMMON_BYTE_ORDER == COMMON_BIG_ENDIAN
    template <typename T> static T hton(T value) { return value; }
    template <typename T> static T ntoh(T value) { return value; }
#elif COMMON_BYTE_ORDER == COMMON_LITTLE_ENDIAN
    template <typename T> static T hton(T value) { return Swap(value); }
    template <typename T> static T ntoh(T value) { return Swap(value); }
#endif
};

template <size_t Size>
struct ByteOrderSwapper : public ByteOrderSwapperBase<Size> {
    using ByteOrderSwapperBase<Size>::Swap;
#if COMMON_BYTE_ORDER == COMMON_BIG_ENDIAN
    template <typename T> static T ToBig(T value) { return value; }
    template <typename T> static T FromBig(T value) { return value; }
    template <typename T> static T ToLittle(T value) { return Swap(value); }
    template <typename T> static T FromLittle(T value) { return Swap(value); }
#elif COMMON_BYTE_ORDER == COMMON_LITTLE_ENDIAN
    template <typename T> static T ToBig(T value) { return Swap(value); }
    template <typename T> static T FromBig(T value) { return Swap(value); }
    template <typename T> static T ToLittle(T value) { return value; }
    template <typename T> static T FromLittle(T value) { return value; }
#endif
};

} // namespace internal

// Byte order conversion.
struct ByteOrder
{
private:
    ByteOrder();
    ~ByteOrder();

public:
    static bool IsBigEndian()
    {
        return COMMON_BYTE_ORDER == COMMON_BIG_ENDIAN;
    }

    static bool IsLittleEndian()
    {
        return COMMON_BYTE_ORDER == COMMON_LITTLE_ENDIAN;
    }

    // Return the conversion result. The ExplicitT must be given explicitly.
    template <typename ExplicitT, typename T>
    static ExplicitT Swap(T value) {
        return internal::ByteOrderSwapper<sizeof(ExplicitT)>::Swap(value);
    }

    // Inplace conversion.
    template <typename T>
    static void Swap(T* value)
    {
        *value = Swap<T>(*value);
    }

    /////////////////////////////////////////////////////////////////////////
    // NetOrder <-> LocalOrder conversion.

    // Return the conversion result. The ExplicitT must be given explicitly.
    template <typename ExplicitT, typename T>
    static ExplicitT ToNet(T value) {
        return internal::ByteOrderSwapper<sizeof(ExplicitT)>::hton(value);
    }

    // Inplace conversion.
    template <typename T>
    static void ToNet(T* value)
    {
        *value = ToNet<T>(*value);
    }

    // Return the conversion result. The ExplicitT must be given explicitly.
    template <typename ExplicitT, typename T>
    static ExplicitT FromNet(T value) {
        return internal::ByteOrderSwapper<sizeof(ExplicitT)>::ntoh(value);
    }

    // Inplace conversion.
    template <typename T>
    static void FromNet(T* value)
    {
        *value = FromNet<T>(*value);
    }

    /////////////////////////////////////////////////////////////////////////
    // BigEndian <-> LocalOrder conversion.

    // Return the conversion result. The ExplicitT must be given explicitly.
    template <typename ExplicitT, typename T>
    static ExplicitT ToBigEndian(T value) {
        return internal::ByteOrderSwapper<sizeof(ExplicitT)>::ToBig(value);
    }

    // Inplace conversion.
    template <typename T>
    static void ToBigEndian(T* value)
    {
        *value = ToBigEndian<T>(*value);
    }

    // Return the conversion result. The ExplicitT must be given explicitly.
    template <typename ExplicitT, typename T>
    static ExplicitT FromBigEndian(T value) {
        return internal::ByteOrderSwapper<sizeof(ExplicitT)>::FromBig(value);
    }

    // Inplace conversion.
    template <typename T>
    static void FromBigEndian(T* value)
    {
        *value = FromBigEndian<T>(*value);
    }

    /////////////////////////////////////////////////////////////////////////
    // LittleEndian <-> LocalOrder conversion.

    // Return the conversion result. The ExplicitT must be given explicitly.
    template <typename ExplicitT, typename T>
    static ExplicitT ToLittleEndian(T value) {
        return internal::ByteOrderSwapper<sizeof(ExplicitT)>::ToLittle(value);
    }

    // Inplace conversion.
    template <typename T>
    static void ToLittleEndian(T* value)
    {
        *value = ToLittleEndian<T>(*value);
    }

    // Return the conversion result. The ExplicitT must be given explicitly.
    template <typename ExplicitT, typename T>
    static ExplicitT FromLittleEndian(T value) {
        return internal::ByteOrderSwapper<sizeof(ExplicitT)>::FromLittle(value);
    }

    // Inplace conversion.
    template <typename T>
    static void FromLittleEndian(T* value)
    {
        *value = FromLittleEndian<T>(*value);
    }
};

} // namespace common

#endif // COMMON_BASE_BYTE_ORDER_H

