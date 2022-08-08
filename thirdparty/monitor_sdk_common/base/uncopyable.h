// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef COMMON_BASE_UNCOPYABLE_H
#define COMMON_BASE_UNCOPYABLE_H

#include "thirdparty/monitor_sdk_common/base/cxx11.h"

//  Private copy constructor and copy assignment ensure classes derived from
//  class Uncopyable cannot be copied.

//  Contributed by Dave Abrahams

namespace common {

/// The macro way
#ifdef COMMON_CXX11_ENABLED
#define COMMON_DECLARE_UNCOPYABLE(Class) \
private: \
    Class(const Class&) = delete; \
    Class& operator=(const Class&) = delete
#else
#define COMMON_DECLARE_UNCOPYABLE(Class) \
private: \
    Class(const Class&); \
    Class& operator=(const Class&)
#endif

/*

Usage:

class Foo {
    COMMON_DECLARE_UNCOPYABLE(Foo);
public:
    Foo();
    ~Foo();
};

*/

/// The private base class way
namespace uncopyable_details  // protection from unintended ADL
{
class Uncopyable
{
    COMMON_DECLARE_UNCOPYABLE(Uncopyable);
protected:
    Uncopyable() {}
    ~Uncopyable() {}
};
} // namespace uncopyable_details

typedef uncopyable_details::Uncopyable Uncopyable;

/*

Usage:

class Foo : private Uncopyable
{
};

*/

} // namespace common

#endif // COMMON_BASE_UNCOPYABLE_H
