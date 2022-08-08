//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-06-21 19:01
//  @file:      sp_counted_impl.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_SMART_PTR_SP_COUNTED_IMPL_H
#define COMMON_BASE_SMART_PTR_SP_COUNTED_IMPL_H

#include "thirdparty/monitor_sdk_common/base/smart_ptr/checked_delete.h"
#include "thirdparty/monitor_sdk_common/base/smart_ptr/detail/sp_counted_base.h"
#include <memory>           // std::allocator
#include <cstddef>          // std::size_t
#include <typeinfo>         // std::type_info

namespace common
{
namespace detail
{

template<class X> class sp_counted_impl_p: public sp_counted_base
{
private:

    X * px_;

    sp_counted_impl_p( sp_counted_impl_p const & );
    sp_counted_impl_p & operator= ( sp_counted_impl_p const & );

    typedef sp_counted_impl_p<X> this_type;

public:

    explicit sp_counted_impl_p( X * px ): px_( px )
    {
    }

    virtual void dispose() // nothrow
    {
        ::common::checked_delete( px_ );
    }

    virtual void * get_deleter( std::type_info const & )
    {
        return 0;
    }


    void * operator new( std::size_t )
    {
        return std::allocator<this_type>().allocate( 1, static_cast<this_type *>(0) );
    }

    void operator delete( void * p )
    {
        std::allocator<this_type>().deallocate( static_cast<this_type *>(p), 1 );
    }


};


template<class P, class D> class sp_counted_impl_pd: public sp_counted_base
{
private:

    P ptr; // copy constructor must not throw
    D del; // copy constructor must not throw

    sp_counted_impl_pd( sp_counted_impl_pd const & );
    sp_counted_impl_pd & operator= ( sp_counted_impl_pd const & );

    typedef sp_counted_impl_pd<P, D> this_type;

public:

    // pre: d(p) must not throw

    sp_counted_impl_pd( P p, D & d ): ptr( p ), del( d )
    {
    }

    sp_counted_impl_pd( P p ): ptr( p ), del()
    {
    }

    virtual void dispose() // nothrow
    {
        del( ptr );
    }

    virtual void * get_deleter( std::type_info const & ti )
    {
        return ti == typeid(D) ? &reinterpret_cast<char&>( del ): 0;
    }


    void * operator new( std::size_t )
    {
        return std::allocator<this_type>().allocate( 1, static_cast<this_type *>(0) );
    }

    void operator delete( void * p )
    {
        std::allocator<this_type>().deallocate( static_cast<this_type *>(p), 1 );
    }

};

template<class P, class D, class A> class sp_counted_impl_pda: public sp_counted_base
{
private:

    P p_; // copy constructor must not throw
    D d_; // copy constructor must not throw
    A a_; // copy constructor must not throw

    sp_counted_impl_pda( sp_counted_impl_pda const & );
    sp_counted_impl_pda & operator= ( sp_counted_impl_pda const & );

    typedef sp_counted_impl_pda<P, D, A> this_type;

public:

    // pre: d( p ) must not throw

    sp_counted_impl_pda( P p, D & d, A a ): p_( p ), d_( d ), a_( a )
    {
    }

    sp_counted_impl_pda( P p, A a ): p_( p ), d_(), a_( a )
    {
    }

    virtual void dispose() // nothrow
    {
        d_( p_ );
    }

    virtual void destroy() // nothrow
    {
        typedef typename A::template rebind< this_type >::other A2;

        A2 a2( a_ );

        this->~this_type();
        a2.deallocate( this, 1 );
    }

    virtual void * get_deleter( std::type_info const & ti )
    {
        return ti == typeid(D) ? &reinterpret_cast<char&>( d_ ): 0;
    }
};

} // namespace detail
} // namespace common

#endif // COMMON_BASE_SMART_PTR_SP_COUNTED_IMPL_H

