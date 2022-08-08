// Copyright (c) 2011, Vivo Inc.
// All rights reserved.
//
// Created: 2011-06-12 20:26:58
// Description:

#include <iostream>
#include <string>
#include "thirdparty/monitor_sdk_common/system/debug/hex_dump.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

static const char kText[] =
    "#include <thirdparty/monitor_sdk_common/system/debug/hex_dump.hpp>\n"
    "#include \"thirdparty/gtest/gtest.h\"\n"
    "\n"
    "int main()\n"
    "{\n"
    "    HexDump((void*) &main, 64, (void*)main);\n"
    "}\n";

static const char kHex[] =
    "0000: 23 69 6E 63 6C 75 64 65-20 3C 63 6F 6D 6D 6F 6E  #include <thirdparty/monitor_sdk_common\n"
    "0010: 2F 73 79 73 74 65 6D 2F-64 65 62 75 67 2F 68 65  /system/debug/he\n"
    "0020: 78 5F 64 75 6D 70 2E 68-70 70 3E 0A 23 69 6E 63  x_dump.hpp>.#inc\n"
    "0030: 6C 75 64 65 20 22 74 68-69 72 64 70 61 72 74 79  lude \"thirdparty\n"
    "0040: 2F 67 74 65 73 74 2F 67-74 65 73 74 2E 68 22 0A  /gtest/gtest.h\".\n"
    "0050: 0A 69 6E 74 20 6D 61 69-6E 28 29 0A 7B 0A 20 20  .int main().{.  \n"
    "0060: 20 20 48 65 78 44 75 6D-70 28 28 76 6F 69 64 2A    HexDump((void*\n"
    "0070: 29 20 26 6D 61 69 6E 2C-20 36 34 2C 20 28 76 6F  ) &main, 64, (vo\n"
    "0080: 69 64 2A 29 6D 61 69 6E-29 3B 0A 7D 0A 00        id*)main);.}..  \n";

TEST(HexDump, DumpToFILE)
{
    HexDump(stdout, kText, sizeof(kText));
}

TEST(HexDump, DumpToStream)
{
    HexDump(std::cout, kText, sizeof(kText));
}

TEST(HexDump, HexDumper)
{
    std::cout << HexDumper(kText, sizeof(kText));
}

TEST(HexDump, ToString)
{
    std::string result;
    HexDumpToString(&result, kText, sizeof(kText));
    EXPECT_EQ(kHex, result);
}

TEST(HexDump, AsString)
{
    std::string result = HexDumpAsString(kText, sizeof(kText));
    EXPECT_EQ(kHex, result);
}

} // namespace common
