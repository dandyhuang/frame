//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:28
//  @file:      intrusive_list_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************

#include "thirdparty/gtest/gtest.h"
#include "thirdparty/monitor_sdk_common/base/stdext/intrusive_list.h"

#include <stdio.h>
#include <assert.h>

namespace common {

struct User
{
    list_node link;
};

TEST(ListTest, List)
{
    intrusive_list<User> l;
    ASSERT_TRUE(l.empty());
    ASSERT_TRUE(l.size() == 0);
    ASSERT_TRUE(l.begin() == l.end());
}

TEST(ListTest, Push)
{
    intrusive_list<User> l;
    User u1, u2, u3;
    l.push_front(u1);
    ASSERT_TRUE(!l.empty());
    ASSERT_EQ(1U, l.size());
    ASSERT_TRUE(l.begin() != l.end());
    ASSERT_TRUE(&*l.begin() == &u1);

    l.push_front(u3);
    ASSERT_TRUE(&*l.begin() == &u3);

    intrusive_list<User>::iterator i = l.begin();
    ++i;
    ASSERT_TRUE(&*i == &u1);
}

TEST(ListTest, Iteration)
{
    intrusive_list<User> l;
    User u1, u2, u3;
    l.push_back(u1);
    l.push_back(u2);
    l.push_back(u3);

    intrusive_list<User>::iterator i;
    i = l.begin();
    ASSERT_EQ(&*i, &u1);
    ++i;
    ASSERT_EQ(&*i, &u2);
    ++i;
    ASSERT_EQ(&*i, &u3);
    ++i;
    ASSERT_TRUE(l.end() == i);
}

TEST(ListTest, Clear)
{
    intrusive_list<User> l;
    User u1, u2, u3;
    l.push_back(u1);
    l.push_back(u2);
    l.push_back(u3);
    ASSERT_FALSE(l.empty());
    ASSERT_TRUE(u1.link.is_linked());
    ASSERT_TRUE(u2.link.is_linked());
    ASSERT_TRUE(u3.link.is_linked());

    l.clear();
    ASSERT_TRUE(l.empty());
    ASSERT_FALSE(u1.link.is_linked());
    ASSERT_FALSE(u2.link.is_linked());
    ASSERT_FALSE(u3.link.is_linked());
}

} // end of namespace common
