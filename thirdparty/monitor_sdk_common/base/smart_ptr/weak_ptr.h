//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-06-21 18:31
//  @file:      weak_ptr.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_SMART_PTR_WEAK_PTR_H
#define COMMON_BASE_SMART_PTR_WEAK_PTR_H


#include "thirdparty/monitor_sdk_common/base/smart_ptr/detail/shared_count.h"
#include "thirdparty/monitor_sdk_common/base/smart_ptr/shared_ptr.h"


namespace common {

template<typename T> 
class weak_ptr
{
private:

    typedef weak_ptr<T> this_type;

public:

    typedef T element_type;

    weak_ptr(): px(0), pn() // never throws in 1.30+
    {
    }


//
//  The "obvious" converting constructor implementation:
//
//  template<class Y>
//  weak_ptr(weak_ptr<Y> const & r): px(r.px), pn(r.pn) // never throws
//  {
//  }
//
//  has a serious problem.
//
//  r.px may already have been invalidated. The px(r.px)
//  conversion may require access to *r.px (virtual inheritance).
//
//  It is not possible to avoid spurious access violations since
//  in multithreaded programs r.px may be invalidated at any point.
//

    template<class Y>
    weak_ptr( weak_ptr<Y> const & r, typename ::common::detail::sp_enable_if_convertible<Y,T>::type = ::common::detail::sp_empty() )
    : px(r.lock().get()), pn(r.pn) // never throws
    {
    }


    template<class Y>
    weak_ptr( shared_ptr<Y> const & r, typename ::common::detail::sp_enable_if_convertible<Y,T>::type = ::common::detail::sp_empty() )
    : px( r.px ), pn( r.pn ) // never throws
    {
    }


    shared_ptr<T> lock() const // never throws
    {
        return shared_ptr<element_type>( *this, ::common::detail::sp_nothrow_tag() );
    }

    long use_count() const // never throws
    {
        return pn.use_count();
    }

    bool expired() const // never throws
    {
        return pn.use_count() == 0;
    }

    bool _empty() const // extension, not in std::weak_ptr
    {
        return pn.empty();
    }

    void reset() // never throws in 1.30+
    {
        this_type().swap(*this);
    }

    void swap(this_type & other) // never throws
    {
        std::swap(px, other.px);
        pn.swap(other.pn);
    }

    void _internal_assign(T * px2, ::common::detail::shared_count const & pn2)
    {
        px = px2;
        pn = pn2;
    }

    template<class Y> bool owner_before( weak_ptr<Y> const & rhs ) const
    {
        return pn < rhs.pn;
    }

    template<class Y> bool owner_before( shared_ptr<Y> const & rhs ) const
    {
        return pn < rhs.pn;
    }


private:

    template<class Y> friend class weak_ptr;
    template<class Y> friend class shared_ptr;

    T * px;                       // contained pointer
    ::common::detail::weak_count pn; // reference counter

};  // weak_ptr

template<class T, class U> inline bool operator<(weak_ptr<T> const & a, weak_ptr<U> const & b)
{
    return a.owner_before( b );
}

template<class T> void swap(weak_ptr<T> & a, weak_ptr<T> & b)
{
    a.swap(b);
}

} // end of namespace common


#endif // COMMON_BASE_SMART_PTR_WEAK_PTR_H

