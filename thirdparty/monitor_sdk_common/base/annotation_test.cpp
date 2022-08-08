// Copyright (c) 2011.
// All rights reserved.
//
// Description:

#include "thirdparty/monitor_sdk_common/base/annotation.h"
#include "thirdparty/gtest/gtest.h"

WARN_UNUSED_RESULT int ResultCanNotBeIgnored()
{
    return 0;
}

TEST(Annotation, ResultCanNotBeIgnored)
{
    // uncomment to test ignore unused result
    // ResultCanNotBeIgnored();

    int n = ResultCanNotBeIgnored();
    (void)n;
    IgnoreUnused(ResultCanNotBeIgnored());
}
