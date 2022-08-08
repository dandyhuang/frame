//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 10:18
//  @file:      mount.h
//  @author:    
//  @brief:     Get a mount list of devices from mount table.
//
//********************************************************************


#ifndef COMMON_SYSTEM_IO_MOUNT_H
#define COMMON_SYSTEM_IO_MOUNT_H
#pragma once

#include <stdint.h>
#include <string>
#include <vector>

namespace common {

struct MountEntry {
    ::std::string device_name;
    ::std::string mount_point;
    ::std::string device_type;
    uint64_t available_size;

    MountEntry() :
        device_name(""),
        mount_point(""),
        device_type(""),
        available_size(0) {}

    MountEntry(::std::string dev_name,
               ::std::string mnt_point,
               ::std::string dev_type,
               uint64_t size_left) :
        device_name(dev_name),
        mount_point(mnt_point),
        device_type(dev_type),
        available_size(size_left) {}
};

bool GetAllMountEntries(::std::vector<MountEntry>* entries);

} // end of namespace common

#endif // COMMON_SYSTEM_IO_MOUNT_H
