// Copyright (c) 2013, The COMMON Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-07

#ifndef COMMON_BASE_STRING_FORMAT_PRINT_TARGET_H
#define COMMON_BASE_STRING_FORMAT_PRINT_TARGET_H
#pragma once

namespace common {

class FormatPrintTarget {
public:
    virtual ~FormatPrintTarget() {}
    virtual void WriteChar(char c) = 0;
    void WriteChars(char c, int count) {
        // Optimize for count == 0
        if (count != 0)
            DoWriteChars(c, count);
    }
    void WriteString(const char* str, int size) {
        // Optimize for size == 0
        if (size != 0)
            DoWriteString(str, size);
    }
private:
    virtual void DoWriteChars(char c, int count) = 0;
    virtual void DoWriteString(const char* str, int size) = 0;
};

} // namespace common

#endif // COMMON_BASE_STRING_FORMAT_PRINT_TARGET_H
