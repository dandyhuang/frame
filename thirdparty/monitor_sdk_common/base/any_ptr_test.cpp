// Copyright (c) 2010.
// All rights reserved.
//

#include "thirdparty/monitor_sdk_common/base/any_ptr.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(AnyPtr, Delete)
{
    AnyPtr p(new int(0));
    p.Delete();
}

} // namespace common
