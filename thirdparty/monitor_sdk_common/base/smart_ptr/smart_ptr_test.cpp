//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-06-21 19:13
//  @file:      smart_ptr_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************

#include "thirdparty/gtest/gtest.h"

#include "thirdparty/monitor_sdk_common/base/smart_ptr/smart_ptr.h"

namespace common {

struct MyTestStruct
{
    int a;
    bool b;
};


TEST(SharedPtrTest, NormalTest) {
    shared_ptr<MyTestStruct> s_ptr(new MyTestStruct);
    s_ptr->a++;
    s_ptr->b = false;
    shared_ptr<MyTestStruct> s_ptr1 = s_ptr;
    EXPECT_EQ(s_ptr->a, s_ptr1->a);
    EXPECT_EQ(s_ptr->b, s_ptr1->b);
}

} // end of namespace common

