//-----------------------------------------------------------------------------
// MurmurHash2A, by Austin Appleby

// This is a variant of MurmurHash2 modified to use the Merkle-Damgard
// construction. Bulk speed should be identical to Murmur2, small-key speed
// will be 10%-20% slower due to the added overhead at the end of the hash.

// This variant fixes a minor issue where null keys were more likely to
// collide with each other than expected, and also makes the algorithm
// more amenable to incremental implementations. All other caveats from
// MurmurHash2 still apply.

#include <stddef.h>
#include <thirdparty/monitor_sdk_common/base/stdint.h>
#include <thirdparty/monitor_sdk_common/crypto/hash/murmur.h>

#define mmix(h,k) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }

namespace common {

uint32_t MurmurHash2A( const void * key, size_t len, uint32_t seed )
{
    const uint32_t m = 0x5bd1e995;
    const int r = 24;
    uint32_t l = len;

    const unsigned char * data = (const unsigned char *)key;

    uint32_t h = seed;

    while (len >= 4)
    {
        uint32_t k = *(uint32_t*)data;

        mmix(h,k);

        data += 4;
        len -= 4;
    }

    uint32_t t = 0;

    switch (len)
    {
    case 3:
        t ^= data[2] << 16;
    case 2:
        t ^= data[1] << 8;
    case 1:
        t ^= data[0];
    };

    mmix(h,t);
    mmix(h,l);

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

void CMurmurHash2A::Begin(uint32_t seed)
{
    m_hash  = seed;
    m_tail  = 0;
    m_count = 0;
    m_size  = 0;
}

void CMurmurHash2A::Add( const void * data, size_t len )
{
    const unsigned char* pdata = static_cast<const unsigned char*>(data);
    m_size += len;

    MixTail(pdata,len);

    while (len >= 4)
    {
        uint32_t k = *(uint32_t*)pdata;

        mmix(m_hash,k);

        pdata += 4;
        len -= 4;
    }

    MixTail(pdata,len);
}

uint32_t CMurmurHash2A::End(void)
{
    mmix(m_hash,m_tail);
    mmix(m_hash,m_size);

    m_hash ^= m_hash >> 13;
    m_hash *= m;
    m_hash ^= m_hash >> 15;

    return m_hash;
}

void CMurmurHash2A::MixTail( const unsigned char * & data, size_t & len )
{
    while ( len && ((len<4) || m_count) )
    {
        m_tail |= (*data++) << (m_count * 8);

        m_count++;
        len--;

        if (m_count == 4)
        {
            mmix(m_hash,m_tail);
            m_tail = 0;
            m_count = 0;
        }
    }
}

} // namespace common
