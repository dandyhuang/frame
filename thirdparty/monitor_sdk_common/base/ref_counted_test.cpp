//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 20:04
//  @file:      ref_counted_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/ref_counted.h"
#include "thirdparty/monitor_sdk_common/base/scoped_refptr.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

namespace {

class SelfAssign : public RefCountedBase<SelfAssign>
{
    friend class RefCountedBase<SelfAssign>;
    ~SelfAssign() {}
};

}  // namespace

TEST(RefCountedTest, Count)
{
    SelfAssign* p = new SelfAssign;
    EXPECT_EQ(1, p->AddRef());
    EXPECT_TRUE(p->IsUnique());

    EXPECT_EQ(2, p->AddRef());
    EXPECT_FALSE(p->IsUnique());

    EXPECT_FALSE(p->Release());

    EXPECT_TRUE(p->IsUnique());
    EXPECT_TRUE(p->Release());
}

} // end of namespace common
