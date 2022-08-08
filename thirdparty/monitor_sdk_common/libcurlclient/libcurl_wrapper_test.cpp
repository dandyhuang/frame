// Copyright (c) 2020, VIVO Inc. All rights reserved.                                                                                  |  1
// Author: Fly Qiu <rock3qiu@gmail.com>                                                                                                 |~
// Created: 2020-01-09                                                                                                                    |~
// Unit test for libcurl_wrapper.

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"
#include "libcurl_wrapper.h"

namespace common {

class LibcurlWrapperTest : public ::testing::Test {
public:
    LibcurlWrapperTest() {
    }

    virtual ~LibcurlWrapperTest() {
    }

    virtual void SetUp() {
    }

    virtual void TearDown() {
    }

protected:
    LibcurlWrapper _libcurl_wrapper;
};

TEST_F(LibcurlWrapperTest, TestNormalHttp) {
    _libcurl_wrapper.init();
    std::map<std::string, std::string> parameters;
    int32_t http_status_code;
    std::string http_header_data;
    std::string http_response_data;
    const std::string url{"http://172.25.197.41:8848/naming/v1/ns/instance/list?serviceName=DEFAULT_GROUP@@mixer-rank-old-proto&namespaceId=test&app=mixer&appKey=ms-proxy"};
    _libcurl_wrapper.send_request(url, parameters,
            &http_status_code, &http_header_data, &http_response_data);

    LOG(ERROR) << "http_status_code " << http_status_code
        << " http_header_data " << " http_response_data " << http_response_data
        << " get_last_error_code " << _libcurl_wrapper.get_last_error_code()
        << " get_last_error_message " << _libcurl_wrapper.get_last_error_message();
    _libcurl_wrapper.reset();

    _libcurl_wrapper.send_request(url, parameters,
            &http_status_code, &http_header_data, &http_response_data);

    LOG(ERROR) << "http_status_code " << http_status_code
        << " http_header_data " << http_header_data << " http_response_data " << http_response_data;
}

TEST_F(LibcurlWrapperTest, TestNormalHttps) {
    _libcurl_wrapper.init();
    std::map<std::string, std::string> parameters;
    int32_t http_status_code;
    std::string http_header_data;
    std::string http_response_data;
    const std::string url{"http://172.25.197.41:8848/naming/v1/ns/instance/list?serviceName=DEFAULT_GROUP@@mixer-rank-old-proto&namespaceId=test&app=mixer&appKey=ms-proxy"};
    _libcurl_wrapper.set_timeout(200, CURL_TIMEOUT_MILLISECOND_TYPE);
    _libcurl_wrapper.send_request(url, parameters,
            &http_status_code, &http_header_data, &http_response_data);

    LOG(INFO) << "http_status_code " << http_status_code
        << " http_header_data "  << " http_response_data " << http_response_data;

    _libcurl_wrapper.reset();
    _libcurl_wrapper.send_request(url, parameters,
            &http_status_code, &http_header_data, &http_response_data);

    LOG(INFO) << "http_status_code " << http_status_code
        << " http_header_data "  << http_header_data << " http_response_data " << http_response_data;
}

} // namespace common

int32_t main(int32_t argc, char* argv[]) {
    // Initialize glog and gflag
    google::InitGoogleLogging(argv[0]);
    // The third argument |false| means do not remove the flags and their arguments from argv.
    google::ParseCommandLineFlags(&argc, &argv, false);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
