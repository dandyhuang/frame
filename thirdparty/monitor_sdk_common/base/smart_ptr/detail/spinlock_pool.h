//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-06-21 18:59
//  @file:      spinlock_pool.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_SMART_PTR_SPINLOCK_POOL_H
#define COMMON_BASE_SMART_PTR_SPINLOCK_POOL_H

//
//  spinlock_pool<0> is reserved for atomic<>, when/if it arrives
//  spinlock_pool<1> is reserved for shared_ptr reference counts
//  spinlock_pool<2> is reserved for shared_ptr atomic access
//

#include "thirdparty/monitor_sdk_common/base/smart_ptr/detail/spinlock.h"
#include <cstddef>


namespace common
{

namespace detail
{

template< int I > class spinlock_pool
{
private:

    static spinlock pool_[ 41 ];

public:

    static spinlock & spinlock_for( void const * pv )
    {
        std::size_t i = reinterpret_cast< std::size_t >( pv ) % 41;
        return pool_[ i ];
    }

    class scoped_lock
    {
    private:

        spinlock & sp_;

        scoped_lock( scoped_lock const & );
        scoped_lock & operator=( scoped_lock const & );

    public:

        explicit scoped_lock( void const * pv ): sp_( spinlock_for( pv ) )
        {
            sp_.lock();
        }

        ~scoped_lock()
        {
            sp_.unlock();
        }
    };
};

template< int I > spinlock spinlock_pool< I >::pool_[ 41 ] =
{
    SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, 
    SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, 
    SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, 
    SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, 
    SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, 
    SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, 
    SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, 
    SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, SOFA_DETAIL_SPINLOCK_INIT, 
    SOFA_DETAIL_SPINLOCK_INIT
};

} // namespace detail
} // namespace common

#endif // COMMON_BASE_SMART_PTR_SPINLOCK_POOL_H

