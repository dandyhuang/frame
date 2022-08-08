// Copyright (c) 2012, Vivo Inc.
// All rights reserved.
//
// Created: 07/09/2012

#include "thirdparty/monitor_sdk_common/crypto/hash/city.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(City, Hash64)
{
    EXPECT_EQ(11160318154034397263UL, CityHash64("", 0));
    EXPECT_EQ(18398641537446792289UL, CityHash64("abcd", 4));
    EXPECT_EQ(2577512742650958718UL, CityHash64("abcdefgh", 8));
    EXPECT_EQ(7063266761827874021UL, CityHash64("abcdefghijklmno", 16));
    EXPECT_EQ(13480628931447031476UL, CityHash64("abcdefghijklmnopqrstuvwxyz", 26));
    EXPECT_EQ(1964039892793292696UL, CityHash64("abcdefghijklmnopqrstuvwxyz123456", 32));
    EXPECT_EQ(3729063051378436287UL, CityHash64("abcdefghijklmnopqrstuvwxyz1234567", 33));
    EXPECT_EQ(8839482714921131865UL,
              CityHash64("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890123", 65));
}

static bool operator==(const common::UInt128& lhs,
                       const common::UInt128& rhs)
{
    return lhs.low == rhs.low && lhs.high == rhs.high;
}

template <typename Char, typename Traits>
static std::basic_ostream<Char, Traits>& operator<<(
    std::basic_ostream<Char, Traits>& os,
    const UInt128& n)
{
    os << "(" << n.low << ", " << n.high << ")";
    return os;
}

TEST(City, Hash128)
{
    EXPECT_EQ(UInt128(2234266707296324255ULL, 12664895223759999564ULL),
              CityHash128("", 0));
    EXPECT_EQ(UInt128(2502809580507936440ULL, 10751026649067943940ULL),
              CityHash128("0123456789", 10));
    EXPECT_EQ(UInt128(17008878195417247351ULL, 15682546298730249978ULL),
              CityHash128("01234567890123456789", 20));
}

} // namespace common
