/**
 * @file Checksum.cc
 * @brief
 * @date 2010-04-29
 */

#ifdef _WIN32
#else
#include <arpa/inet.h>
#endif
#include <zlib.h>
#include "thirdparty/monitor_sdk_common/compress/checksum/checksum.h"

namespace common {

#define INTERN_F32_DO1(buf,i) \
    sum1 += ((uint16_t)buf[i] << 8) | buf[i+1]; sum2 += sum1

#define INTERN_F32_DO2(buf,i)  INTERN_F32_DO1(buf,i); INTERN_F32_DO1(buf,i+2);
#define INTERN_F32_DO4(buf,i)  INTERN_F32_DO2(buf,i); INTERN_F32_DO2(buf,i+4);
#define INTERN_F32_DO8(buf,i)  INTERN_F32_DO4(buf,i); INTERN_F32_DO4(buf,i+8);
#define INTERN_F32_DO16(buf,i) INTERN_F32_DO8(buf,i); INTERN_F32_DO8(buf,i+16);

/* cf. http://en.wikipedia.org/wiki/Fletcher%27s_checksum
*/

uint32_t fletcher32(const void *data8, size_t len8)
{

    /* data may not be aligned properly and would segfault on
     * many systems if cast and used as 16-bit words
     */
    const uint8_t *data = (const uint8_t *)data8;
    uint32_t sum1 = 0xffff, sum2 = 0xffff;
    size_t len = len8 / 2; /* loop works on 16-bit words */

    while (len) {
        /* 360 is the largest number of sums that can be
         * performed without integer overflow
         */
        unsigned tlen = len > 360 ? 360 : len;
        len -= tlen;

        if (tlen >= 16) do {
            INTERN_F32_DO16(data, 0);
            data += 32;
            tlen -= 16;
        } while (tlen >= 16);

        if (tlen != 0) do {
            INTERN_F32_DO1(data, 0);
            data += 2;
        } while (--tlen);

        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }

    /* Check for odd number of bytes */
    if (len8 & 1) {
        sum1 += ((uint16_t)*data) << 8;
        sum2 += sum1;
        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }

    /* Second reduction step to reduce sums to 16 bits */
    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    return (sum2 << 16) | sum1;
}

#define INTERN_F32A_DO1(buf, i) sum1 += ntohs(buf[i]); sum2 += sum1;
#define INTERN_F32A_DO2(buf,i)  INTERN_F32A_DO1(buf,i); INTERN_F32A_DO1(buf,i+1);
#define INTERN_F32A_DO4(buf,i)  INTERN_F32A_DO2(buf,i); INTERN_F32A_DO2(buf,i+2);
#define INTERN_F32A_DO8(buf,i)  INTERN_F32A_DO4(buf,i); INTERN_F32A_DO4(buf,i+4);
#define INTERN_F32A_DO16(buf,i) INTERN_F32A_DO8(buf,i); INTERN_F32A_DO8(buf,i+8);

uint32_t fletcher32a(const void *pdata, size_t len)
{
    const uint16_t* data = static_cast<const uint16_t*>(pdata);
    uint32_t sum1 = 0xffff, sum2 = 0xffff;

    while (len) {
        /* 360 is the largest number of sums that can be
         * performed without integer overflow
         */
        unsigned tlen = len > 360 ? 360 : len;
        len -= tlen;

        if (tlen >= 16) do {
            INTERN_F32A_DO16(data, 0);
            data += 16;
            tlen -= 16;
        } while (tlen >= 16);

        if (tlen != 0) do {
            sum1 += ntohs(*data);
            ++data;
            sum2 += sum1;
        } while (--tlen);

        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }
    /* Second reduction step to reduce sums to 16 bits */
    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    return (sum2 << 16) | sum1;
}


/* cf. http://en.wikipedia.org/wiki/Adler-32
*/
#define MOD_ADLER 65521

inline uint32_t adler32_update_wp(uint32_t adler, const void *data8, size_t len)
{
    const uint8_t *data = (const uint8_t *)data8;
    uint32_t a = adler & 0xffff, b = (adler >> 16) & 0xffff;

    while (len) {
        /* see the wikipedia page for why 5550 is used
         *  instead of 5552 mentioned in the RFC
         */
        size_t tlen = len > 5550 ? 5550 : len;
        len -= tlen;
        do {
            a += *data++;
            b += a;
        } while (--tlen);

        a = (a & 0xffff) + (a >> 16) * (65536 - MOD_ADLER);
        b = (b & 0xffff) + (b >> 16) * (65536 - MOD_ADLER);
    }

    /* It can be shown that a <= 0x1013a here, so a single subtract will do. */
    if (a >= MOD_ADLER)
        a -= MOD_ADLER;

    /* It can be shown that b can reach 0xfff87 here. */
    b = (b & 0xffff) + (b >> 16) * (65536 - MOD_ADLER);

    if (b >= MOD_ADLER)
        b -= MOD_ADLER;

    return (b << 16) | a;
}

uint32_t adler32_wp(const void *data, size_t len)
{
    return adler32_update_wp(1, data, len);
}

#define INTERN_A32_DO1(buf,i)  a += buf[i]; b += a
#define INTERN_A32_DO2(buf,i)  INTERN_A32_DO1(buf,i); INTERN_A32_DO1(buf,i+1);
#define INTERN_A32_DO4(buf,i)  INTERN_A32_DO2(buf,i); INTERN_A32_DO2(buf,i+2);
#define INTERN_A32_DO8(buf,i)  INTERN_A32_DO4(buf,i); INTERN_A32_DO4(buf,i+4);
#define INTERN_A32_DO16(buf,i) INTERN_A32_DO8(buf,i); INTERN_A32_DO8(buf,i+8);

inline uint32_t adler32_update(uint32_t adler, const void *data8, size_t len)
{
    const uint8_t *data = (const uint8_t *)data8;
    uint32_t a = adler & 0xffff, b = (adler >> 16) & 0xffff;

    while (len) {
        size_t tlen = len > 5552 ? 5552 : len;
        len -= tlen;

        if (tlen >= 16) do {
            INTERN_A32_DO16(data, 0);
            data += 16;
            tlen -= 16;
        } while (tlen >= 16);

        if (tlen != 0) do {
            a += *data++;
            b += a;
        } while (--tlen > 0);

        a %= MOD_ADLER;
        b %= MOD_ADLER;
    }
    return (b << 16) | a;
}

uint32_t adler32(const void *data, size_t len)
{
    return adler32_update(1, data, len);
}

uint32_t crc32(const void *data, size_t len)
{
    uint32_t crc = ::crc32(0LL, Z_NULL, 0);
    return ::crc32(crc, (Bytef *)data, len);
}

uint32_t crc32_update(uint32_t crc, const void *data, size_t len)
{
    return ::crc32(crc, (Bytef *)data, len);
}

} // namespace common

