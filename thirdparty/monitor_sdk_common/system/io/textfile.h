//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:46
//  @file:      textfile.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_IO_TEXTFILE_H
#define COMMON_SYSTEM_IO_TEXTFILE_H
#pragma once

#include <string>
#include <vector>
#include <deque>
#include <list>
#include "thirdparty/monitor_sdk_common/base/stdint.h"

namespace common {

namespace io { namespace textfile {

bool LoadToString(const ::std::string& filename,
                  ::std::string* result,
                  int64_t max_size = 128 *1024 * 1024);
bool ReadLines(const ::std::string& filename, ::std::vector< ::std::string>* result);
bool ReadLines(const ::std::string& filename, ::std::deque< ::std::string>* result);
bool ReadLines(const ::std::string& filename, ::std::list< ::std::string>* result);

} }

} // end of namespace common

#endif // COMMON_SYSTEM_IO_TEXTFILE_H
