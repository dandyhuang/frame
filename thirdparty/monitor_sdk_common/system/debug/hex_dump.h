// Copyright (c) 2011, Vivo Inc.
// All rights reserved.
//
// Created: 2011-06-12 20:15:45
// Description:

#ifndef COMMON_SYSTEM_DEBUG_HEX_DUMP_H
#define COMMON_SYSTEM_DEBUG_HEX_DUMP_H

/// @file
/// @details
/// example:
/// 0000: 23 69 6E 63 6C 75 64 65-20 3C 63 6F 6D 6D 6F 6E  #include <commo
/// 0010: 2F 73 79 73 74 65 6D 2F-64 65 62 75 67 2F 68 65  /system/debug/h
/// 0020: 78 5F 64 75 6D 70 2E 68-70 70 3E 0A 23 69 6E 63  x_dump.hpp>.#in
/// 0040: 74 2E 68 3E 0A 0A 69 6E-74 20 6D 61 69 6E 28 29  t.h>..int main(
/// 0050: 0A 7B 0A 20 20 20 20 48-65 78 44 75 6D 70 28 28  .{.    HexDump(
/// 0070: 2C 20 28 76 6F 69 64 2A-29 6D 61 69 6E 29 3B 0A  , (void*)main);

#include <stddef.h>
#include <stdio.h>

#include <iosfwd>
#include <string>

namespace common {

/// @brief dump memory in hex format.
/// @param fp FILE* object to be dumped to
/// @param buffer buffer address to be dumped
/// @param size buffer size
/// @param print_address whether printf buffer address
void HexDump(FILE* fp, const void* buffer, size_t size, bool print_address = false);

/// @brief dump memory in hex format.
/// @param fd fd to be dumped to
/// @param buffer buffer address to be dumped
/// @param size buffer size
/// @param print_address whether printf buffer address
void HexDump(int fd, const void* buffer, size_t size, bool print_address = false);

/// @brief dump memory in hex format.
/// @param os ostream to be dumped to
/// @param buffer buffer address to be dumped
/// @param size buffer size
/// @param print_address whether printf buffer address
void HexDump(std::ostream& os, const void* buffer, size_t size, bool print_address = false);

/// @brief dump memory in hex format to string.
/// @param s string to be dumped to
/// @param buffer buffer address to be dumped
/// @param size buffer size
/// @param print_address whether printf buffer address
void HexDumpToString(std::string* s, const void* buffer, size_t size, bool print_address = false);

/// @brief dump memory in hex format to string.
/// @param s string to be appended to
/// @param buffer buffer address to be dumped
/// @param size buffer size
/// @param print_address whether printf buffer address
void HexDumpAppendString(
    std::string* s,
    const void* buffer, size_t size,
    bool print_address = false
);

/// @brief dump memory in hex format as string.
/// @param buffer buffer address to be dumped
/// @param size buffer size
/// @param print_address whether printf buffer address
/// @return the result string
std::string HexDumpAsString(const void* buffer, size_t size, bool print_address = false);

class HexDumper
{
public:
    HexDumper(const void* buffer, size_t size, bool print_address = false):
        m_buffer(buffer), m_size(size), m_print_address(print_address)
    {
    }
    void Dump(std::ostream& os) const
    {
        HexDump(os, m_buffer, m_size, m_print_address);
    }
private:
    const void* m_buffer;
    size_t m_size;
    bool m_print_address;
};

/// @brief Allow output to C++ stream or GLOG:
///  std::cout << HexDumper(text, sizeof(text));
///  LOG(INFO) << '\n' << HexDumper(text, sizeof(text));
inline std::ostream& operator<<(std::ostream& os, const HexDumper& hex_dumper)
{
    hex_dumper.Dump(os);
    return os;
}

} // namespace common

#endif // COMMON_SYSTEM_DEBUG_HEX_DUMP_H
