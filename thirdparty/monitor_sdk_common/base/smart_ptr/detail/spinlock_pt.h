//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-06-21 18:57
//  @file:      spinlock_pt.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_SMART_PTR_SPINLOCK_PT_H
#define COMMON_BASE_SMART_PTR_SPINLOCK_PT_H


#include <pthread.h>


namespace common
{

namespace detail
{

class spinlock
{
public:

    pthread_mutex_t v_;

public:

    bool try_lock()
    {
        return pthread_mutex_trylock( &v_ ) == 0;
    }

    void lock()
    {
        pthread_mutex_lock( &v_ );
    }

    void unlock()
    {
        pthread_mutex_unlock( &v_ );
    }

public:

    class scoped_lock
    {
    private:

        spinlock & sp_;

        scoped_lock( scoped_lock const & );
        scoped_lock & operator=( scoped_lock const & );

    public:

        explicit scoped_lock( spinlock & sp ): sp_( sp )
        {
            sp.lock();
        }

        ~scoped_lock()
        {
            sp_.unlock();
        }
    };
};

} // namespace detail
} // namespace common

#define SOFA_DETAIL_SPINLOCK_INIT { PTHREAD_MUTEX_INITIALIZER }
#endif // COMMON_BASE_SMART_PTR_SPINLOCK_PT_H


