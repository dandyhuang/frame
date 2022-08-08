//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-06-21 18:52
//  @file:      enable_shared_from_this.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_SMART_PTR_ENABLE_SHARED_FROM_THIS_H
#define COMMON_BASE_SMART_PTR_ENABLE_SHARED_FROM_THIS_H


#include "thirdparty/monitor_sdk_common/base/smart_ptr/weak_ptr.h"
#include "thirdparty/monitor_sdk_common/base/smart_ptr/shared_ptr.h"


namespace common
{

template<class T> class enable_shared_from_this
{
protected:

    enable_shared_from_this()
    {
    }

    enable_shared_from_this(enable_shared_from_this const &)
    {
    }

    enable_shared_from_this & operator=(enable_shared_from_this const &)
    {
        return *this;
    }

    ~enable_shared_from_this()
    {
    }

public:

    shared_ptr<T> shared_from_this()
    {
        shared_ptr<T> p( weak_this_ );
        assert( p.get() == this );
        return p;
    }

    shared_ptr<T const> shared_from_this() const
    {
        shared_ptr<T const> p( weak_this_ );
        assert( p.get() == this );
        return p;
    }

public: // actually private, but avoids compiler template friendship issues

    // Note: invoked automatically by shared_ptr; do not call
    template<class X, class Y> void _internal_accept_owner( shared_ptr<X> const * ppx, Y * py ) const
    {
        if( weak_this_.expired() )
        {
            weak_this_ = shared_ptr<T>( *ppx, py );
        }
    }

private:

    mutable weak_ptr<T> weak_this_;
};

} // namespace common

#endif // COMMON_BASE_SMART_PTR_ENABLE_SHARED_FROM_THIS_H

