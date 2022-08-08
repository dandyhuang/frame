//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 10:19
//  @file:      mount.cpp
//  @author:    
//  @brief:     Get a mount list of devices from mount table.
//
//********************************************************************

#include "thirdparty/monitor_sdk_common/system/io/mount.h"
#include <mntent.h>
#include <string.h>
#include <sys/vfs.h>

namespace common {

// The mntent struct :
//
// struct mntent {
// char *mnt_fsname;
// char *mnt_dir;
// char *mnt_type;
// char *mnt_opts;
// int mnt_freq;
// int mnt_passno;
// };
//
bool GetAllMountEntries(::std::vector<MountEntry>* entries) {
    FILE* mount_table = NULL;
    struct mntent* mount_entry = NULL;
    struct statfs mount_point_stat;

    mount_table = setmntent("/etc/mtab", "r");
    if (mount_table == NULL)
        return false;

    uint64_t size_left = 0;
    while ((mount_entry = getmntent(mount_table)) != NULL) {
        // skip rootfs according to coreutils
        if (strncmp("rootfs", mount_entry->mnt_fsname, 6) == 0 ) {
            continue;
        }

        if (statfs(mount_entry->mnt_dir, &mount_point_stat) == -1) {
            size_left = 0;
        } else {
            size_left = mount_point_stat.f_bsize * mount_point_stat.f_bavail;
        }

        entries->push_back(MountEntry(mount_entry->mnt_fsname,
                                      mount_entry->mnt_dir,
                                      mount_entry->mnt_type,
                                      size_left));
    }

    endmntent(mount_table);
    return true;
}


} // end of namespace common
