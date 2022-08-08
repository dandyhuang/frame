//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-06-21 18:44
//  @file:      owner_less.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef BASS_COMMON_BASE_SMART_PTR_OWNER_LESS_H
#define BASS_COMMON_BASE_SMART_PTR_OWNER_LESS_H

#include <functional>


namespace common
{
  template<typename T> class shared_ptr;
  template<typename T> class weak_ptr;

  namespace detail
  {
    template<typename T, typename U>
      struct generic_owner_less : public std::binary_function<T, T, bool>
    {
      bool operator()(const T &lhs, const T &rhs) const
      {
        return lhs.owner_before(rhs);
      }
      bool operator()(const T &lhs, const U &rhs) const
      {
        return lhs.owner_before(rhs);
      }
      bool operator()(const U &lhs, const T &rhs) const
      {
        return lhs.owner_before(rhs);
      }
    };
  } // namespace detail

  template<typename T> struct owner_less;

  template<typename T>
    struct owner_less<shared_ptr<T> >:
    public detail::generic_owner_less<shared_ptr<T>, weak_ptr<T> >
  {};

  template<typename T>
    struct owner_less<weak_ptr<T> >:
    public detail::generic_owner_less<weak_ptr<T>, shared_ptr<T> >
  {};

} // namespace common

#endif  // #ifndef BASS_COMMON_BASE_SMART_PTR_OWNER_LESS_H
