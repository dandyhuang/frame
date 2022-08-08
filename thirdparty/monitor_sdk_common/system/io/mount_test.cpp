//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 10:20
//  @file:      mount_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/io/mount.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(MountTest, GetMountPointOfFileSystem)
{
    ::std::string mount_dir = "";
    ::std::vector<MountEntry> entries;
    ::std::vector<MountEntry>::const_iterator iter;

    ASSERT_TRUE(GetAllMountEntries(&entries)) << "failed to open mount table";
    //for(iter = entries.begin(); iter != entries.end(); ++iter) {
    //    if (iter->device_name.find("udev") != std::string::npos) {
    //        mount_dir = iter->mount_point;
    //        break;
    //    }
    //}

    //EXPECT_NE("", mount_dir);
}

} // end of namespace common
