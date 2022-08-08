//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 20:07
//  @file:      string.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_STDEXT_STRING_H
#define COMMON_BASE_STDEXT_STRING_H

#include <string>

namespace common {

// Return a mutable char* pointing to a string's internal buffer,
// which may not be null-terminated. Writing through this pointer will
// modify the string.
//
// string_as_array(&str)[i] is valid for 0 <= i < str.size() until the
// next call to a string method that invalidates iterators.
//
// As of 2006-04, there is no standard-blessed way of getting a
// mutable reference to a string's internal buffer. However, issue 530
// (http://www.open-std.org/JTC1/SC22/WG21/docs/lwg-active.html#530)
// proposes this as the method. According to Matt Austern, this should
// already work on all current implementations.
inline char* string_as_array(::std::string* str)
{
    // DO NOT USE const_cast<char*>(str->data())!
    //
    // Many string implementations using reference count to share string
    // content between string instances. Since data() and c_str() are
    // both readonly attributes, they may return the pointer to shared
    // data. Writing to the shared data will demage other string objects.
    //
    // A non-const begin call guarantee its data always become unique
    // and then safe for writing.
    return str->empty() ? NULL : &*str->begin();
}

} // end of namespace common

#endif // COMMON_BASE_STDEXT_STRING_H
