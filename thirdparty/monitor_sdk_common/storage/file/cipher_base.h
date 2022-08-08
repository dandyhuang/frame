// Copyright (c) 2015, Vivo Inc. All rights reserved.
// All rights reserved.
// Created: 11/04/2013 06:21:16 PM

#ifndef COMMON_FILE_CIPHER_BASE_H
#define COMMON_FILE_CIPHER_BASE_H

#include <stdint.h>

#include <string>

#include "thirdparty/monitor_sdk_common/base/class_register.h"
#include "thirdparty/monitor_sdk_common/base/closure2.h"
#include "thirdparty/glog/logging.h"

namespace common {

class CipherBase;

// Following Macros are for registering and creating cipher implementations.

CLASS_REGISTER_DEFINE_REGISTRY(cipher_impl_register, CipherBase);

#define REGISTER_CIPHER_IMPL(entry_name_as_str, file_impl_name) \
    CLASS_REGISTER_OBJECT_CREATOR_WITH_SINGLETON( \
    cipher_impl_register, CipherBase, entry_name_as_str, file_impl_name)

#define CREATE_CIPHER_IMPL(entry_name_as_str) \
    CLASS_REGISTER_CREATE_OBJECT(cipher_impl_register, entry_name_as_str)


#define CIPHER_IMPL_COUNT() \
    CLASS_REGISTER_CREATOR_COUNT(cipher_impl_register)

#define CIPHER_IMPL_NAME(i) \
    CLASS_REGISTER_CREATOR_NAME(cipher_impl_register, i)



class CipherBase {
public:
    static CipherBase* CreateCipher() {
        if (CIPHER_IMPL_COUNT() == 0U) {
            LOG(ERROR) << "no cipher implementation is initialized. ";
            return NULL;
        }

        CipherBase* impl_ret = NULL;
        std::string impl_name;
        for (size_t i = 0; i < CIPHER_IMPL_COUNT(); ++i) {
            const std::string& cur_name = CIPHER_IMPL_NAME(i);
            LOG(INFO) << "cipher impl[" << cur_name << "] initialized!";
            // return the last registered implementation
            impl_name = cur_name;
        }
        impl_ret = CREATE_CIPHER_IMPL(impl_name);

        LOG(INFO) << "CreateCipher ret=" << impl_name
            << ", impl_cnt=" << CIPHER_IMPL_COUNT();
        return impl_ret;
    }

    CipherBase() {}
    virtual ~CipherBase() {}
    virtual bool Open_Write(const char* filename,
                Closure<int64_t, const void* , int64_t , uint32_t*>* real_write,
                Closure<int32_t, uint32_t*>* real_flush) = 0;

    virtual bool Open_Read(const char* filename,
                Closure<int64_t, void* , int64_t , uint32_t*>* real_read) = 0;

    virtual int64_t Write(const void* buffer, int64_t size,
                uint32_t* error_code = NULL) = 0;

    virtual int64_t Read(void* buffer, int64_t size, uint32_t* error_code = NULL) = 0;

    virtual int32_t Flush(uint32_t* error_code) = 0;

    // clean up dynamically allocated buffer, which can be revoked in Close()
    // It can avoid large memory leak, if user forget to delete File* Object at which case
    // the destructor has not been called;
    virtual void Cleanup() = 0;
};

} // namespace common

#endif // COMMON_FILE_CIPHER_BASE_H
