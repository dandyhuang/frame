//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-06-21 18:53
//  @file:      checked_delete.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_SMART_PTR_CHECKED_DELETE_H
#define COMMON_BASE_SMART_PTR_CHECKED_DELETE_H

namespace common {

template <typename T>
inline void checked_delete(T* px)
{
    typedef char type_must_be_complete[sizeof(T) ? 1 : -1 ];
    (void) sizeof(type_must_be_complete);
    delete px;
}

template<class T> inline void checked_array_delete(T * x)
{
    typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
    (void) sizeof(type_must_be_complete);
    delete [] x;
}

template<class T> struct checked_deleter
{
    typedef void result_type;
    typedef T * argument_type;

    void operator()(T * x) const
    {
        ::common::checked_delete(x);
    }
};

template<class T> struct checked_array_deleter
{
    typedef void result_type;
    typedef T * argument_type;

    void operator()(T * x) const
    {
        ::common::checked_array_delete(x);
    }
};

} // namespace common


#endif // COMMON_BASE_SMART_PTR_CHECKED_DELETE_H


