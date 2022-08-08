/*
 * tst_uuid.c --- test program from the UUID library
 *
 * Copyright (C) 1996, 1997, 1998 Theodore Ts'o.
 *
 * %Begin-Header%
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, and the entire permission notice in its entirety,
 *    including the disclaimer of warranties.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ALL OF
 * WHICH ARE HEREBY DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF NOT ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * %End-Header%
 */

#ifdef _WIN32
#define _WIN32_WINNT 0x0500
#include <windows.h>
#define UUID MYUUID
#endif

#include <stdio.h>
#include <stdlib.h>

#include "thirdparty/monitor_sdk_common/system/uuid/uuid.h"
#include "thirdparty/gtest/gtest.h"


namespace common {

static void print_hex(FILE* fp, const void* buf, size_t length)
{
    unsigned char *cp = (unsigned char *) buf;
    for (size_t i = 0; i < length; ++i) {
        printf("%02x", cp[i]);
    }
}

TEST(UUID, Generate)
{
    uuid_t      buf;
    char        str[100];

    uuid_generate(buf);
    uuid_unparse(buf, str);
    printf("UUID generate = %s\n", str);
    printf("UUID: ");

    print_hex(stdout, buf, 16);
    printf("\n");

    EXPECT_EQ(UUID_VARIANT_DCE, uuid_variant(buf)) <<
        "Incorrect UUID Variant; was expecting DCE!";

    uuid_generate_random(buf);
    uuid_unparse(buf, str);
    printf("UUID random string = %s\n", str);
    printf("UUID: ");
    print_hex(stdout, buf, 16);
    printf("\n");

    EXPECT_EQ(UUID_VARIANT_DCE, uuid_variant(buf)) <<
        "Incorrect UUID Variant; was expecting DCE!";
    EXPECT_EQ(4, uuid_type(buf)) <<
        "Incorrect UUID type; was expecting 4 (random type)!";
}

TEST(UUID, GenerateTime)
{
    uuid_t buf, tst;
    uuid_generate_time(buf);

    char str[100];
    uuid_unparse(buf, str);
    printf("UUID string = %s\n", str);
    printf("UUID time: ");

    print_hex(stdout, buf, 16);
    printf("\n");

    EXPECT_EQ(UUID_VARIANT_DCE, uuid_variant(buf)) <<
        "Incorrect UUID Variant; was expecting DCE!";

    EXPECT_EQ(1, uuid_type(buf)) <<
        "Incorrect UUID type; was expecting 1 (time-based type)!";

    struct timeval  tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    time_t time_reg = uuid_time(buf, &tv);
    printf("UUID time is: (%ld, %ld): %s\n", tv.tv_sec, tv.tv_usec,
           ctime(&time_reg));
    uuid_parse(str, tst);
    EXPECT_EQ(0, uuid_compare(buf, tst)) <<
        "UUID parse and compare failed!";
}

TEST(UUID, Clear)
{
    uuid_t tst;
    uuid_clear(tst);
    EXPECT_TRUE(uuid_is_null(tst));
}

TEST(UUID, Copy)
{
    uuid_t buf, tst;
    uuid_copy(buf, tst);
    EXPECT_EQ(0, uuid_compare(buf, tst));
}

static bool test_parse(const char * uuid)
{
    uuid_t uuidBits;
    return uuid_parse(uuid, uuidBits) == 0;
}

TEST(UUID, Parse)
{
    EXPECT_TRUE(test_parse("84949cc5-4701-4a84-895b-354c584a981b"));
    EXPECT_TRUE(test_parse("84949CC5-4701-4A84-895B-354C584A981B"));
    EXPECT_FALSE(test_parse("84949cc5-4701-4a84-895b-354c584a981bc"));
    EXPECT_FALSE(test_parse("84949cc5-4701-4a84-895b-354c584a981"));
    EXPECT_FALSE(test_parse("84949cc5x4701-4a84-895b-354c584a981b"));
    EXPECT_FALSE(test_parse("84949cc504701-4a84-895b-354c584a981b"));
    EXPECT_FALSE(test_parse("84949cc5-470104a84-895b-354c584a981b"));
    EXPECT_FALSE(test_parse("84949cc5-4701-4a840895b-354c584a981b"));
    EXPECT_FALSE(test_parse("84949cc5-4701-4a84-895b0354c584a981b"));
    EXPECT_FALSE(test_parse("g4949cc5-4701-4a84-895b-354c584a981b"));
    EXPECT_FALSE(test_parse("84949cc5-4701-4a84-895b-354c584a981g"));
}

TEST(UUID, Unparse)
{
    uuid_t uuidBits;
    EXPECT_EQ(0, uuid_parse("84949cc5-4701-4a84-895b-354c584a981b", uuidBits));
    char buf[37];
    uuid_unparse(uuidBits, buf);
    EXPECT_EQ("84949cc5-4701-4a84-895b-354c584a981b", std::string(buf, 36));
    uuid_unparse_lower(uuidBits, buf);
    EXPECT_EQ("84949cc5-4701-4a84-895b-354c584a981b", std::string(buf, 36));
    uuid_unparse_upper(uuidBits, buf);
    EXPECT_EQ("84949CC5-4701-4A84-895B-354C584A981B", std::string(buf, 36));
}

TEST(UUID, Uuid)
{
    Uuid uuid;
    EXPECT_TRUE(uuid.IsNull());
    uuid.Generate();
    EXPECT_FALSE(uuid.IsNull());

    Uuid uuid2(uuid);
    EXPECT_TRUE(uuid == uuid2);
    uuid.Generate();
    EXPECT_TRUE(uuid != uuid2);

    EXPECT_FALSE(uuid.Parse("84949cc5-4701-4a84-895b-354c584a981bc"));
    EXPECT_TRUE(uuid.Parse("84949cc5-4701-4a84-895b-354c584a981b"));
    EXPECT_EQ("84949cc5-4701-4a84-895b-354c584a981b", uuid.ToString());
}

} // end of namespace common
