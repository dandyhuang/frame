//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:54
//  @file:      rwlock_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/rwlock.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(RWLock, Kind)
{
    RWLock lock1(RWLock::kKindPreferReader);
    RWLock lock2(RWLock::kKindPreferWriter);
    RWLock lock3(RWLock::kKindDefault);
}

TEST(RWLock, AccessDestructedCheckDeathTest)
{
    //RWLock* plock;
    //{
    //    RWLock lock;
    //    plock = &lock;
    //}
    //EXPECT_DEATH(plock->ReaderLock(), "Invalid argument");
    //EXPECT_DEATH(plock->ReaderLock(), "Invalid argument");
    //EXPECT_DEATH(plock->WriterLock(), "Invalid argument");
    //EXPECT_DEATH(plock->TryReaderLock(), "Invalid argument");
    //EXPECT_DEATH(plock->TryWriterLock(), "Invalid argument");
    //EXPECT_DEATH(plock->Unlock(), "Invalid argument");
}

} // end of namespace common
