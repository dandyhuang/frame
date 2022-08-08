// Copyright (c) 2011, Vivo Inc.
// All rights reserved.
//
// Created: 2011-06-12 19:45:35
// Description:

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <ostream>
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/base/uncopyable.h"

#ifdef __unix__
#include <unistd.h>
#endif

namespace common {

namespace {

class FilePrinter
{
    COMMON_DECLARE_UNCOPYABLE(FilePrinter);
public:
    explicit FilePrinter(FILE* fp) : m_fp(fp) {}
    void Print(const char* s)
    {
        fprintf(m_fp, "%s", s);
    }
private:
    FILE* m_fp;
};

#ifdef _MSC_VER
#define write _write
#endif

class FdPrinter
{
    COMMON_DECLARE_UNCOPYABLE(FdPrinter);
public:
    explicit FdPrinter(int fd) : m_fd(fd) {}
    void Print(const char* s)
    {
        if (write(m_fd, s, strlen(s)) < 0) {
            // Iagnore error.
            // But return value should be checked for glibc2.14+.
        }
    }
private:
    int m_fd;
};

class OStreamPrinter
{
    COMMON_DECLARE_UNCOPYABLE(OStreamPrinter);
public:
    explicit OStreamPrinter(std::ostream& os) : m_os(os) {}
    void Print(const char* s)
    {
        m_os << s;
    }
private:
    std::ostream& m_os;
};

class StringPrinter
{
    COMMON_DECLARE_UNCOPYABLE(StringPrinter);
public:
    explicit StringPrinter(std::string* str) : m_str(str) {}
    void Print(const char* s)
    {
        m_str->append(s);
    }
private:
    std::string* m_str;
};

template <typename PrinterType>
void HexDumpTemplate(
    PrinterType* printer,
    const void* buffer,
    size_t size,
    bool print_address
)
{
    const int kAddressWidth = 6;   // '0000: ', 5 characters and 1 space
    const int kBytesPerLine = 16;
    const int kWidthPerByte = 3;    // 'F0 ', 2 characters and 1 space

    static const char kHexAlphabet[] = "0123456789ABCDEF";
    const unsigned char* data = static_cast<const unsigned char*>(buffer);

    uint32_t address = 0;
    if (print_address)
    {
        printf("Starting address: %p\n", buffer);
        address = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(buffer));
    }

    size_t char_start_pos =
        kAddressWidth +
        kBytesPerLine * kWidthPerByte + 1;

    for (size_t i = 0; i < size; i += kBytesPerLine)
    {
        // DebugView display each DbgPrint output in one line,
        // so I gather them in one line
        char line[
            kAddressWidth +  // Address width
            kBytesPerLine * (kWidthPerByte + 1)  // contents
            + 3  // tail NULL
        ];

        uint32_t a = address + i;
        // only print the low 2 byte of address
        line[0] = kHexAlphabet[(a>>12) & 0x0F];
        line[1] = kHexAlphabet[(a>>8) & 0x0F];
        line[2] = kHexAlphabet[(a>>4) & 0x0F];
        line[3] = kHexAlphabet[(a) & 0x0F];
        line[4] = ':';
        line[5] = ' ';

        // print the hex-char delimiter
        line[char_start_pos - 1] = ' ';

        for (int j = 0; j < kBytesPerLine; ++j)
        {
            size_t hex_pos = kAddressWidth + j * kWidthPerByte;
            size_t char_pos = char_start_pos + j;

            if (i + j < size)
            {
                unsigned char c = data[i + j];
                line[hex_pos + 0] = kHexAlphabet[c >> 4];
                line[hex_pos + 1] = kHexAlphabet[c & 0x0F];
                line[hex_pos + 2] = ' ';
                line[char_pos] = isprint(c) ? c : '.';

                // print middle indicator
                if (j == 8)
                    line[hex_pos - 1] = '-';
            }
            else
            {
                line[hex_pos + 0] = ' ';
                line[hex_pos + 1] = ' ';
                line[hex_pos + 2] = ' ';
                line[char_pos] = ' ';
            }
        }

        size_t end_pos = kAddressWidth + kBytesPerLine * (kWidthPerByte + 1) + 1;
        line[end_pos] = '\n';

        line[sizeof(line)-1] = 0;
        printer->Print(line);
    }
}

} // end anonymous namespace

void HexDump(FILE* fp, const void* buffer, size_t size, bool print_address)
{
    FilePrinter printer(fp);
    HexDumpTemplate(&printer, buffer, size, print_address);
}

void HexDump(int fd, const void* buffer, size_t size, bool print_address)
{
    FdPrinter printer(fd);
    HexDumpTemplate(&printer, buffer, size, print_address);
}

void HexDump(std::ostream& os, const void* buffer, size_t size, bool print_address)
{
    OStreamPrinter printer(os);
    HexDumpTemplate(&printer, buffer, size, print_address);
}

void HexDumpAppendString(std::string* s, const void* buffer, size_t size, bool print_address)
{
    StringPrinter printer(s);
    HexDumpTemplate(&printer, buffer, size, print_address);
}

void HexDumpToString(std::string* s, const void* buffer, size_t size, bool print_address)
{
    s->clear();
    HexDumpAppendString(s, buffer, size, print_address);
}

std::string HexDumpAsString(const void* buffer, size_t size, bool print_address)
{
    std::string result;
    HexDumpAppendString(&result, buffer, size, print_address);
    return result;
}

} // namespace common
