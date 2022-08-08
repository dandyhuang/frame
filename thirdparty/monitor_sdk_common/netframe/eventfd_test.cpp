// Copyright (c) 2011.
// All rights reserved.
//
//
// Created: 06/28/11
// Description:

#include "thirdparty/monitor_sdk_common/netframe/eventfd.h"
#include <stdio.h>
#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/thread.h"


static void Producer(int evfd, int loop_count)
{
    for (int i = 0; i < loop_count; ++i)
    {
        eventfd_t l = i;
        eventfd_write(evfd, l);
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <loop_count>.\n", argv[0]);
        return 1;
    }
    unsigned int loop_count;
    if (!common::StringToNumber(argv[1], &loop_count)) {
        fprintf(stderr, "loop count is not a integer.\n");
        return 1;
    }

    int evfd = eventfd(0, 0);
    if (evfd < 0)
    {
        perror("eventfd");
        return 1;
    }

    Producer(evfd, loop_count);
    close(evfd);
}

