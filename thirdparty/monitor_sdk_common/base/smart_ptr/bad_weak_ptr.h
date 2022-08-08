//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-06-21 18:55
//  @file:      bad_weak_ptr.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_SMART_PTR_BAD_WEAK_PTR_H
#define COMMON_BASE_SMART_PTR_BAD_WEAK_PTR_H

#include <exception>


namespace common
{

class bad_weak_ptr: public std::exception
{
public:

    virtual char const * what() const throw()
    {
        return "::common::bad_weak_ptr";
    }
};

} // namespace common

#endif // COMMON_BASE_SMART_PTR_BAD_WEAK_PTR_H

