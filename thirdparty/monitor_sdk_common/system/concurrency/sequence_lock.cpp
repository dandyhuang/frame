//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 15:19
//  @file:      sequence_lock.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/sequence_lock.h"

namespace common {

SequenceLock::SequenceLock() : m_sequence(0)
{
}

SequenceLock::~SequenceLock()
{
}

} // end of namespace common
