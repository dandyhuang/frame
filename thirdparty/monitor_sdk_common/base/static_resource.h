// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 09/30/11
// Description: a handy resource convert macro

#ifndef COMMON_BASE_STATIC_RESOURCE_H
#define COMMON_BASE_STATIC_RESOURCE_H
#pragma once

#include "thirdparty/monitor_sdk_common/base/string/string_piece.h"

/// @brief convert a static array to static resource StringPiece
/// @param name name of the resource
///
/// example:
/// StringPiece icon = COMMON_STATIC_RESOURCE(poppy_favicon_ico);
/// RegisterStaticResource("/favicon.ico", COMMON_STATIC_RESOURCE(poppy_favicon_ico))
///
#define COMMON_STATIC_RESOURCE(name) \
    StringPiece(RESOURCE_##name, sizeof(RESOURCE_##name))

// Blade generate struct
#ifndef BLADE_RESOURCE_TYPE_DEFINED
#define BLADE_RESOURCE_TYPE_DEFINED
struct BladeResourceEntry {
    const char* name;
    const char* data;
    unsigned int size;
};
#endif

namespace common {

// Each blade resource_library defines a static resource package,
// this class provide assess interface by file name.
class StaticResourcePackage {
public:
    StaticResourcePackage(const BladeResourceEntry* entry, unsigned length);
    bool Find(StringPiece name, StringPiece* data) const;
private:
    const BladeResourceEntry* m_resources;
    unsigned int m_length;
};

template <const BladeResourceEntry* entry, const unsigned int* length>
class ConstStaticResourcePackage : public StaticResourcePackage {
private:
    ConstStaticResourcePackage() : StaticResourcePackage(entry, *length) {}
public:
    static const StaticResourcePackage& Instance() {
        // Mayers singleton
        static const ConstStaticResourcePackage package;
        return package;
    }
};

// Static resource package access macro
#define COMMON_STATIC_RESOURCE_PACKAGE(name) \
    ::common::ConstStaticResourcePackage<RESOURCE_INDEX_##name, \
                                       &RESOURCE_INDEX_##name##_len>::Instance()

} // namespace common

#endif // COMMON_BASE_STATIC_RESOURCE_H
