// Copyright (c) 2020, VIVO Inc. All rights reserved.                                                                                  |  1
// Author: Fly Qiu <rock3qiu@gmail.com>                                                                                                 |~
// Created: 2020-01-09                                                                                                                    |~

#include "libcurl_wrapper.h"
#include <stdlib.h> // for atexit
#include "thirdparty/glog/logging.h"
#include "global_initialize.h"

namespace common
{

// The function curl_global_init is not thread-safe(can not be invoked by multi threads at the
// same time). So init curl before entering the main entry.
// More information refer to http://linux.die.net/man/3/curl_global_init.
    GLOBAL_INITIALIZE(curl_global_init)
    {
        CHECK(curl_global_init(CURL_GLOBAL_ALL) == 0) << "Curl global init failed";
        atexit(curl_global_cleanup);
    }

    pthread_mutex_t LibcurlWrapper::_s_curl_lock = PTHREAD_MUTEX_INITIALIZER;

    LibcurlWrapper::LibcurlWrapper()
            : _last_error_code(CURLE_OK),
              _last_error_message("NO Error"),
              _form_post(NULL),
              _last_ptr(NULL),
              _header_list(NULL),
              _timeout(0),
              _max_download_speed(0)
    {
        _curl = curl_easy_init();
        set_share_handle(_curl);
    }

    LibcurlWrapper::~LibcurlWrapper()
    {
        if (_header_list != NULL)
        {
            curl_slist_free_all(_header_list);
        }

        if (_form_post != NULL)
        {
            curl_formfree(_form_post);
        }

        if (_last_ptr != NULL)
        {
            curl_formfree(_last_ptr);
        }

        curl_easy_cleanup(_curl);
    }

    bool LibcurlWrapper::init()
    {
        if (_curl == NULL)
        {
            _last_error_message = "curl easy init failed";
            return false;
        }

        return true;
    }

    bool LibcurlWrapper::reset()
    {
        _last_error_code = CURLE_OK;
        _last_error_message = "NO Error";

        if (_header_list != NULL)
        {
            curl_slist_free_all(_header_list);
        }

        if (_form_post != NULL)
        {
            curl_formfree(_form_post);
        }

        if (_last_ptr != NULL)
        {
            curl_formfree(_last_ptr);
        }

        _timeout = 0;
        _max_download_speed = 0;
        _host = "";
        _user_name = "";
        _password = "";

        return true;
    }

    bool LibcurlWrapper::set_timeout(int timeout, CURL_TIMEOUT_TYPE type)
    {
        //LOG(INFO) << "Set timeout to " << timeout_ms << " ms";
        _timeout = timeout;
        _timeout_type = type;

        return true;
    }
	
    bool LibcurlWrapper::set_connect_timeout(int timeout, CURL_TIMEOUT_TYPE type)
    {
        //LOG(INFO) << "Set timeout to " << timeout_ms << " ms";
        _connect_timeout = timeout;
        _connect_timeout_type = type;

        return true;
    }

    bool LibcurlWrapper::set_max_download_speed(int32_t speed)
    {
        LOG(INFO) << "Set max_download_speed to " << speed << " B/s";
        _max_download_speed = speed;
        return true;
    }

    bool LibcurlWrapper::set_proxy(
            const std::string &host, const std::string &user, const std::string &password)
    {
        // Set proxy information if necessary.
        if (!host.empty())
        {
            _host = host;
        }
        else
        {
            LOG(ERROR) << "set_proxy called with empty proxy host.";
            return false;
        }

        if (!user.empty())
        {
            _user_name = user;
            _password = password;
        }
        else
        {
            LOG(ERROR) << "set_proxy called with empty proxy username/password.";
            return false;
        }

        LOG(INFO) << "Set proxy host to " << host;
        return true;
    }

    bool LibcurlWrapper::set_headers(const std::set<std::string> &headers)
    {
        if (headers.empty())
        {
            LOG(ERROR) << "headers is empty!";
            return false;
        }

        std::set<std::string>::const_iterator head_iter = headers.begin();
        for (; head_iter != headers.end(); ++head_iter)
        {
            _header_list = curl_slist_append(_header_list, (*head_iter).c_str());
        }

        return true;
    }

    bool LibcurlWrapper::set_range(const std::string &range)
    {
        if (!range.empty())
        {
            // Based on my test, curl_easy_setopt(_curl, CURLOPT_RANGE, range.c_str()) will add
            // Content-range NOT Range in http request header, which is not we want.
            _header_list = curl_slist_append(
                    _header_list, (std::string("Range: bytes=") + range).c_str());
        }
        else
        {
            LOG(ERROR) << "set_range called with empty range.";
            return false;
        }

        LOG(INFO) << "Set range to " << range;
        return true;
    }

    bool LibcurlWrapper::add_file(const std::string &file_path, const std::string &base_name)
    {
        LOG(INFO) << "Adding " << file_path << " to form upload.";
        // Add form file.
        curl_formadd(&_form_post, &_last_ptr,
                     CURLFORM_COPYNAME, base_name.c_str(),
                     CURLFORM_FILE, file_path.c_str(), CURLFORM_END);

        return true;
    }

// Callback to get the response data from server.
    static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *user_data)
    {
        size_t real_size = size * nmemb;

        if (user_data != NULL)
        {
            std::string *response = reinterpret_cast<std::string *>(user_data);
            response->append(ptr, real_size);
        }

        return real_size;
    }

    bool LibcurlWrapper::send_request(
            const std::string &url, const std::map<std::string, std::string> &parameters, int32_t *http_status_code
            , std::string* http_header_data, std::string *http_response_data)
    {
        curl_easy_reset(_curl);
        //curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
        std::map<std::string, std::string>::const_iterator iter = parameters.begin();
        for (; iter != parameters.end(); ++iter)
        {
            curl_formadd(&_form_post, &_last_ptr,
                         CURLFORM_COPYNAME, iter->first.c_str(),
                         CURLFORM_COPYCONTENTS, iter->second.c_str(), CURLFORM_END);
        }

        if (!parameters.empty())
        {
            curl_easy_setopt(_curl, CURLOPT_HTTPPOST, _form_post);
        }
        if (http_response_data != NULL)
        {
            http_response_data->clear();
            curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(_curl, CURLOPT_WRITEDATA, reinterpret_cast<void *>(http_response_data));
        }
        if (http_header_data != NULL)
        {
            http_header_data->clear();
            curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, write_callback);
            curl_easy_setopt(_curl, CURLOPT_HEADERDATA, reinterpret_cast<void *>(http_header_data));
        }

        curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _header_list);

        if (_max_download_speed > 0)
        {
            curl_easy_setopt(_curl, CURLOPT_MAX_RECV_SPEED_LARGE,
                             static_cast<long>(_max_download_speed));
        }

        if (!_host.empty() && !_user_name.empty() && !_password.empty())
        {
            curl_easy_setopt(_curl, CURLOPT_PROXY, _host.c_str());
            curl_easy_setopt(_curl, CURLOPT_PROXYUSERPWD, (_user_name + ":" + _password).c_str());
        }

        // Telling libcurl to skip all signal handling.
        // For more information, please refer to:
        //     http://curl.haxx.se/libcurl/c/CURLOPT_NOSIGNAL.html
        // and
        //     http://curl.haxx.se/libcurl/c/CURLOPT_TIMEOUT.html
        curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1L);
        set_timeout();
        set_connect_timeout();

        _last_error_code = curl_easy_perform(_curl);
        if (http_status_code != NULL)
        {
            long codep = 0;
            curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &codep);
            *http_status_code = static_cast<int>(codep);
        }

        if (_last_error_code != CURLE_OK)
        {
            _last_error_message = curl_easy_strerror(_last_error_code);
            //LOG(ERROR) << "Failed to send http request to " << url.c_str()
            //    << ", error: " << curl_easy_strerror(_last_error_code);
        }

        return _last_error_code == CURLE_OK;
    }

    bool LibcurlWrapper::send_post_request(
            const std::string &url, const std::string &body, int32_t *http_status_code, std::string *http_header_data
            , std::string *http_response_data)
    {
        curl_easy_reset(_curl);
        //curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());

        curl_easy_setopt(_curl, CURLOPT_HTTPPOST, _form_post);
        curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE_LARGE, body.size());

        if (http_response_data != NULL)
        {
            http_response_data->clear();
            curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(_curl, CURLOPT_WRITEDATA, reinterpret_cast<void *>(http_response_data));
        }
        if (http_header_data != NULL)
        {
            http_header_data->clear();
            curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, write_callback);
            curl_easy_setopt(_curl, CURLOPT_HEADERDATA, reinterpret_cast<void *>(http_header_data));
        }

        curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);

        curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _header_list);

        if (_max_download_speed > 0)
        {
            curl_easy_setopt(_curl, CURLOPT_MAX_RECV_SPEED_LARGE,
                             static_cast<long>(_max_download_speed));
        }

        if (!_host.empty() && !_user_name.empty() && !_password.empty())
        {
            curl_easy_setopt(_curl, CURLOPT_PROXY, _host.c_str());
            curl_easy_setopt(_curl, CURLOPT_PROXYUSERPWD, (_user_name + ":" + _password).c_str());
        }

        // Telling libcurl to skip all signal handling.
        // For more information, please refer to:
        //     http://curl.haxx.se/libcurl/c/CURLOPT_NOSIGNAL.html
        // and
        //     http://curl.haxx.se/libcurl/c/CURLOPT_TIMEOUT.html
        curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1L);
        set_timeout();
        set_connect_timeout();

        _last_error_code = curl_easy_perform(_curl);
        if (http_status_code != NULL)
        {
            long codep = 0;
            curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &codep);
            *http_status_code = static_cast<int>(codep);
        }

        if (_last_error_code != CURLE_OK)
        {
            _last_error_message = curl_easy_strerror(_last_error_code);
            //LOG(ERROR) << "Failed to send http request to " << url.c_str()
            //    << ", error: " << curl_easy_strerror(_last_error_code);
        }

        return _last_error_code == CURLE_OK;
    }

    bool LibcurlWrapper::send_get_request(
            const std::string &url, int32_t *http_status_code, std::string *http_header_data
            , std::string *http_response_data)
    {
        curl_easy_reset(_curl);
        //curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());

        if (http_response_data != NULL)
        {
            http_response_data->clear();
            curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(_curl, CURLOPT_WRITEDATA, reinterpret_cast<void *>(http_response_data));
        }
        if (http_header_data != NULL)
        {
            http_header_data->clear();
            curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, write_callback);
            curl_easy_setopt(_curl, CURLOPT_HEADERDATA, reinterpret_cast<void *>(http_header_data));
        }

        curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);

        curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _header_list);

        if (_max_download_speed > 0)
        {
            curl_easy_setopt(_curl, CURLOPT_MAX_RECV_SPEED_LARGE,
                             static_cast<long>(_max_download_speed));
        }

        if (!_host.empty() && !_user_name.empty() && !_password.empty())
        {
            curl_easy_setopt(_curl, CURLOPT_PROXY, _host.c_str());
            curl_easy_setopt(_curl, CURLOPT_PROXYUSERPWD, (_user_name + ":" + _password).c_str());
        }

        // Telling libcurl to skip all signal handling.
        // For more information, please refer to:
        //     http://curl.haxx.se/libcurl/c/CURLOPT_NOSIGNAL.html
        // and
        //     http://curl.haxx.se/libcurl/c/CURLOPT_TIMEOUT.html
        curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1L);
        set_timeout();
        set_connect_timeout();

        _last_error_code = curl_easy_perform(_curl);
        if (http_status_code != NULL)
        {
            long codep = 0;
            curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &codep);
            *http_status_code = static_cast<int>(codep);
        }

        if (_last_error_code != CURLE_OK)
        {
            _last_error_message = curl_easy_strerror(_last_error_code);
            LOG(ERROR) << "Failed to send http request to " << url.c_str()
                       << ", error: " << curl_easy_strerror(_last_error_code);
        }

        return _last_error_code == CURLE_OK;
    }

    bool LibcurlWrapper::send_put_request(
            const std::string &url, const std::string &body, int32_t *http_status_code, std::string *http_header_data
            , std::string *http_response_data)
    {
        return send_request(url, body, "PUT", http_status_code, http_header_data, http_response_data);
    }

    bool LibcurlWrapper::send_delete_request(
            const std::string &url, const std::string &body, int32_t *http_status_code, std::string *http_header_data
            , std::string *http_response_data)
    {
        return send_request(url, body, "DELETE", http_status_code, http_header_data, http_response_data);
    }

    int32_t LibcurlWrapper::get_last_error_code()
    {
        return static_cast<int32_t>(_last_error_code);
    }

    std::string LibcurlWrapper::get_last_error_message()
    {
        return _last_error_message;
    }

    bool LibcurlWrapper::set_share_handle(CURL *curl_handle)
    {
        static CURLSH *share_handle = NULL;
        if (share_handle == NULL)
        {
            share_handle = curl_share_init();
            CURLSHcode share_error = curl_share_setopt(
                    share_handle,
                    CURLSHOPT_LOCKFUNC,
                    curl_share_lock_callback);
            if (share_error != CURLSHE_OK)
            {
                LOG(ERROR) << "share set opt CURLSHOPT_LOCKFUNC wrong";
                return false;
            }

            share_error = curl_share_setopt(
                    share_handle,
                    CURLSHOPT_UNLOCKFUNC,
                    curl_share_unlock_callback);
            if (share_error != CURLSHE_OK)
            {
                LOG(ERROR) << "share set opt CURLSHOPT_UNLOCKFUNC wrong";
                return false;
            }

            share_error = curl_share_setopt(
                    share_handle,
                    CURLSHOPT_SHARE,
                    CURL_LOCK_DATA_DNS);
            if (share_error != CURLSHE_OK)
            {
                LOG(ERROR) << "share set opt CURL_LOCK_DATA_DNS wrong";
                return false;
            }
        }
        curl_easy_setopt(curl_handle, CURLOPT_SHARE, share_handle);

        return true;
    }
	

    void LibcurlWrapper::set_timeout()
    {
        if (_timeout > 0)
        {
            switch (_timeout_type)
            {
                case CURL_TIMEOUT_SECOND_TYPE:
                    curl_easy_setopt(_curl, CURLOPT_TIMEOUT, static_cast<long>(_timeout));
                    break;
                case CURL_TIMEOUT_MILLISECOND_TYPE:
                    curl_easy_setopt(_curl, CURLOPT_TIMEOUT_MS, static_cast<long>(_timeout));
                    break;
                default:
                    curl_easy_setopt(_curl, CURLOPT_TIMEOUT_MS, static_cast<long>(DEFAULT_CURL_TIMEOUT_MS));
                    break;
            }
        }
        else
        {
            curl_easy_setopt(_curl, CURLOPT_TIMEOUT_MS, static_cast<long>(DEFAULT_CURL_TIMEOUT_MS));
        }
    }
	
    void LibcurlWrapper::set_connect_timeout()
    {
        if (_timeout > 0)
        {
            switch (_timeout_type)
            {
                case CURL_TIMEOUT_SECOND_TYPE:
                    curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT, static_cast<long>(_timeout));
                    break;
                case CURL_TIMEOUT_MILLISECOND_TYPE:
                    curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT_MS, static_cast<long>(_timeout));
                    break;
                default:
                    curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT_MS, static_cast<long>(DEFAULT_CURL_TIMEOUT_MS));
                    break;
            }
        }
        else
        {
            curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT_MS, static_cast<long>(DEFAULT_CURL_TIMEOUT_MS));
        }
    }

    bool LibcurlWrapper::send_request(const std::string &url, const std::string &body, const std::string &method, int32_t *http_status_code, std::string *http_header_data
                      , std::string *http_response_data)
    {
        curl_easy_reset(_curl);
        //curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());

        if (http_response_data != NULL)
        {
            http_response_data->clear();
            curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(_curl, CURLOPT_WRITEDATA, reinterpret_cast<void *>(http_response_data));
        }
        if (http_header_data != NULL)
        {
            http_header_data->clear();
            curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, write_callback);
            curl_easy_setopt(_curl, CURLOPT_HEADERDATA, reinterpret_cast<void *>(http_header_data));
        }

        curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);

        curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _header_list);

        curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, method.c_str());

        if (_max_download_speed > 0)
        {
            curl_easy_setopt(_curl, CURLOPT_MAX_RECV_SPEED_LARGE,
                             static_cast<long>(_max_download_speed));
        }

        if (!_host.empty() && !_user_name.empty() && !_password.empty())
        {
            curl_easy_setopt(_curl, CURLOPT_PROXY, _host.c_str());
            curl_easy_setopt(_curl, CURLOPT_PROXYUSERPWD, (_user_name + ":" + _password).c_str());
        }

        // Telling libcurl to skip all signal handling.
        // For more information, please refer to:
        //     http://curl.haxx.se/libcurl/c/CURLOPT_NOSIGNAL.html
        // and
        //     http://curl.haxx.se/libcurl/c/CURLOPT_TIMEOUT.html
        curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1L);

        set_timeout();
        set_connect_timeout();
        if (!body.empty())
        {
            curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, body.c_str());
        }

        _last_error_code = curl_easy_perform(_curl);
        if (http_status_code != NULL)
        {
            long codep = 0;
            curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &codep);
            *http_status_code = static_cast<int>(codep);
        }

        if (_last_error_code != CURLE_OK)
        {
            _last_error_message = curl_easy_strerror(_last_error_code);
            LOG(ERROR) << "Failed to send http request to " << url.c_str()
                       << ", error: " << curl_easy_strerror(_last_error_code);
        }

        return _last_error_code == CURLE_OK;
    }

} // namespace common
