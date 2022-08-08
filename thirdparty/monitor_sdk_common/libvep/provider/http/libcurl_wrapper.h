// Copyright (c) 2020, VIVO Inc. All rights reserved.                                                                                  |  1
// Author: Fly Qiu <rock3qiu@gmail.com>                                                                                                 |~
// Created: 2020-01-09                                                                                                                    |~
// A wrapper class for libcurl.

#ifndef COMMON_LIBVEP_LIBCURL_WRAPPER_H_
#define COMMON_LIBVEP_LIBCURL_WRAPPER_H_

#include <map>
#include <set>
#include <string>
#include "curl/curl.h"

namespace common {

class LibcurlWrapper {
public:
    LibcurlWrapper();
    virtual ~LibcurlWrapper();

    virtual bool init();

    virtual bool reset();

    virtual bool set_timeout(int timeout_ms);

    virtual bool set_max_download_speed(int32_t speed);

    virtual bool set_proxy(
            const std::string& host,
            const std::string& user,
            const std::string& password);

    virtual bool set_range(const std::string& range);

    virtual bool set_headers(const std::set<std::string>& headers);

    virtual bool add_file(
            const std::string& file_path,
            const std::string& base_name);

    // 重复调用send_request 复用的是同样的header 因此如果header有变化 先reset再set_headers
    virtual bool send_request(
            const std::string& url,
            const std::map<std::string, std::string>& parameters,
            int32_t* http_status_code,
            std::string* http_header_data,
            std::string* http_response_data);

    virtual bool send_post_request(
            const std::string& url,
            const std::string& body,
            int32_t* http_status_code,
            std::string* http_header_data,
            std::string* http_response_data);

    virtual bool send_get_request(
            const std::string& url,
            int32_t* http_status_code,
            std::string* http_header_data,
            std::string* http_response_data);

    virtual int32_t get_last_error_code();

    virtual std::string get_last_error_message();

    virtual bool set_share_handle(CURL* curl_handle);
public:
     // curl_lock is used for libcurl share interfaces
    static void curl_share_lock_callback(
            CURL* handle,
            curl_lock_data data,
            curl_lock_access access,
            void* userptr) {
        (void)handle;
        (void)access;
        pthread_mutex_lock(&_s_curl_lock);
    }

    static void curl_share_unlock_callback(
            CURL *handle,
            curl_lock_data data,
            void* userptr) {
        (void)handle;
        pthread_mutex_unlock(&_s_curl_lock);
    }

private:
    CURLcode _last_error_code;
    std::string _last_error_message;  // The text of the last error when dealing with CURL.
    CURL* _curl; // Pointer for handle for CURL calls.

    // Stateful pointers for calling into curl_formadd()
    struct curl_httppost* _form_post;
    struct curl_httppost* _last_ptr;
    struct curl_slist* _header_list;

    // params
    int32_t _timeout_ms;
    int32_t _max_download_speed;
    std::string _host;
    std::string _user_name;
    std::string _password;

    // for libcurl
    static pthread_mutex_t _s_curl_lock;
};

} // namespace common

#endif // COMMON_LIBVEP_LIBCURL_WRAPPER_H_
