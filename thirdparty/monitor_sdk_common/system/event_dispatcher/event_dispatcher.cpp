// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "thirdparty/monitor_sdk_common/system/event_dispatcher/event_dispatcher.h"
#include "thirdparty/libev/ev++.h"

namespace common {

EventDispatcher::EventDispatcher() : m_loop(ev_loop_new()) {
}

EventDispatcher::~EventDispatcher() {
    ev_loop_destroy(m_loop);
}

} // namespace common
