//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-22 15:00
//  @file:      memory_cache_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/collection/memory_cache.h"
//#include "thirdparty/monitor_sdk_common/base/stdext/shared_ptr.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/this_thread.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(MemoryCache, Capacity)
{
    MemoryCache<int, int> cache(3);
    EXPECT_EQ(3U, cache.Capacity());
}

TEST(MemoryCache, Size)
{
    MemoryCache<int, int> cache(3);
    EXPECT_EQ(0U, cache.Size());
    EXPECT_TRUE(cache.IsEmpty());

    cache.Put(1, 0);
    EXPECT_EQ(1U, cache.Size());
    EXPECT_FALSE(cache.IsEmpty());
    EXPECT_FALSE(cache.IsFull());

    cache.Put(2, 0);
    EXPECT_EQ(2U, cache.Size());
    EXPECT_FALSE(cache.IsEmpty());
    EXPECT_FALSE(cache.IsFull());

    cache.Put(3, 0);
    EXPECT_EQ(3U, cache.Size());
    EXPECT_FALSE(cache.IsEmpty());
    EXPECT_TRUE(cache.IsFull());

    cache.Put(4, 0);
    EXPECT_EQ(3U, cache.Size());
    EXPECT_FALSE(cache.IsEmpty());
    EXPECT_TRUE(cache.IsFull());
}

TEST(MemoryCache, Insert)
{
    MemoryCache<int, int> cache(3);
    EXPECT_TRUE(cache.Insert(1, 0));
    EXPECT_TRUE(cache.Insert(2, 0));
    EXPECT_FALSE(cache.Insert(1, 0));
    EXPECT_FALSE(cache.Insert(2, 0));
}

TEST(MemoryCache, InsertWithExpiry)
{
    MemoryCache<int, int> cache(3);
    EXPECT_TRUE(cache.InsertWithExpiry(1, 0, 1));
    ThisThread::Sleep(5);
    int value = 0;
    EXPECT_FALSE(cache.Get(1, &value));
}

TEST(MemoryCache, Replace)
{
    MemoryCache<int, int> cache(3);
    EXPECT_TRUE(cache.Insert(1, 0));
    EXPECT_TRUE(cache.Insert(2, 0));
    EXPECT_TRUE(cache.Replace(1, 0));
    EXPECT_TRUE(cache.Replace(2, 0));
    EXPECT_FALSE(cache.Replace(3, 0));
}

TEST(MemoryCache, ReplaceWithExpiry)
{
    MemoryCache<int, int> cache(3);
    EXPECT_TRUE(cache.Insert(1, 0));
    EXPECT_TRUE(cache.ReplaceWithExpiry(1, 0, 1));
    ThisThread::Sleep(5);
    int value = 0;
    EXPECT_FALSE(cache.Get(1, &value));
}

TEST(MemoryCache, Put)
{
    MemoryCache<int, int> cache(3);
    EXPECT_TRUE(cache.Put(1, 1));
    EXPECT_TRUE(cache.Put(1, 1));
    EXPECT_TRUE(cache.Put(2, 1));
    EXPECT_TRUE(cache.Put(3, 1));
    EXPECT_TRUE(cache.Put(4, 1));
    EXPECT_TRUE(cache.Put(5, 1));
}

TEST(MemoryCache, PutWithExpiry)
{
    MemoryCache<int, int> cache(3);
    EXPECT_TRUE(cache.PutWithExpiry(1, 1, 1));
    ThisThread::Sleep(5);
    int value = 0;
    EXPECT_FALSE(cache.Get(1, &value));
}

TEST(MemoryCache, PutWithLongerExpiry)
{
    MemoryCache<int, int> cache(3);
    EXPECT_TRUE(cache.PutWithExpiry(1, 1, 1));
    EXPECT_TRUE(cache.PutWithExpiry(2, 2, 1000000));
    ThisThread::Sleep(5);
    int value = 0;
    EXPECT_FALSE(cache.Get(1, &value));
    EXPECT_TRUE(cache.Get(2, &value));
    EXPECT_EQ(2, value);
}

TEST(MemoryCache, PutWithShorterExpiry)
{
    MemoryCache<int, int> cache(3);
    EXPECT_TRUE(cache.PutWithExpiry(1, 1, 1000000));
    int value = 0;
    EXPECT_TRUE(cache.Get(1, &value));
    EXPECT_EQ(1, value);
    EXPECT_TRUE(cache.PutWithExpiry(1, 1, 1));
    ThisThread::Sleep(5);
    EXPECT_FALSE(cache.Get(1, &value));
}

TEST(MemoryCache, Get)
{
    MemoryCache<int, int> cache(3);
    EXPECT_TRUE(cache.Put(1, 1));

    int value = 0;
    EXPECT_TRUE(cache.Get(1, &value));
    EXPECT_EQ(1, value);

    EXPECT_FALSE(cache.Get(0, &value));
}

TEST(MemoryCache, GetOrDefault)
{
    MemoryCache<int, int> cache(3);
    EXPECT_TRUE(cache.Put(1, 1));
    EXPECT_EQ(1, cache.GetOrDefault(1));
    EXPECT_EQ(0, cache.GetOrDefault(2));
}

TEST(MemoryCache, Remove)
{
    MemoryCache<int, int> cache(3);
    EXPECT_FALSE(cache.Remove(0));
    cache.Put(0, 1);
    EXPECT_TRUE(cache.Remove(0));
    EXPECT_FALSE(cache.Remove(0));
}

TEST(MemoryCache, Discard)
{
    MemoryCache<int, int> cache(3);
    cache.Put(1, 0);
    cache.Put(2, 0);
    cache.Put(3, 0);
    cache.Put(4, 0);
    EXPECT_TRUE(cache.Contains(4));
    EXPECT_TRUE(cache.Contains(3));
    EXPECT_TRUE(cache.Contains(2));
    EXPECT_FALSE(cache.Contains(1));
    EXPECT_EQ(1U, cache.Discard(1));
    EXPECT_EQ(2U, cache.Discard(5));
}

TEST(MemoryCache, Clear)
{
    MemoryCache<int, int> cache(3);
    cache.Put(1, 0);
    EXPECT_FALSE(cache.IsEmpty());
    cache.Clear();
    EXPECT_TRUE(cache.IsEmpty());
}

TEST(MemoryCache, Iteration)
{
    MemoryCache<int, int> cache(3);
    cache.Put(1, 0);
    cache.Put(2, 0);
    cache.Put(3, 0);
    cache.Put(4, 0);

    int key = 0, value = 0;
    ASSERT_TRUE(cache.First(&key, &value));
    printf("key = %d, value = %d\n", key, value);

    ASSERT_TRUE(cache.Next(&key, &value));
    printf("key = %d, value = %d\n", key, value);

    ASSERT_TRUE(cache.Next(&key, &value));
    printf("key = %d, value = %d\n", key, value);

    ASSERT_FALSE(cache.Next(&key, &value));
}

TEST(MemoryCache, InsertPerformance)
{
    MemoryCache<int, int> cache(10000);
    for (int i = 0; i < 1000000; ++i)
        cache.Put(i, i);
}

TEST(MemoryCache, Dump) {
    MemoryCache<int, int> cache(4);
    cache.Put(1, 1);
    cache.Put(2, 2);
    cache.Put(3, 3);
    cache.Put(4, 4);
    std::map<int, int> t_map;
    cache.Dump(&t_map);
    ASSERT_EQ(cache.Size(), t_map.size());
    EXPECT_EQ(1, t_map[1]);
    EXPECT_EQ(2, t_map[2]);
    EXPECT_EQ(3, t_map[3]);
    EXPECT_EQ(4, t_map[4]);
}

//class TestValue
//{
//public:
//    TestValue() { ++s_count; }
//    TestValue(const TestValue&) { ++s_count; }
//    ~TestValue() { --s_count; }
//    static size_t Count() { return s_count; }
//private:
//    static size_t s_count;
//};
//
//size_t TestValue::s_count = 0;
//
//TEST(MemoryCache, SharedPtrValue)
//{
//    {
//        MemoryCache<int, stdext::shared_ptr<TestValue> > cache(10000);
//        for (int i = 0; i < 1000; ++i)
//            cache.Put(i, stdext::shared_ptr<TestValue>(new TestValue()));
//        EXPECT_EQ(1000U, TestValue::Count());
//        for (int i = 0; i < 1000; ++i)
//            cache.Put(i, stdext::shared_ptr<TestValue>(new TestValue()));
//        EXPECT_EQ(1000U, TestValue::Count());
//    }
//    EXPECT_EQ(0U, TestValue::Count());
//}

} // end of namespace common
