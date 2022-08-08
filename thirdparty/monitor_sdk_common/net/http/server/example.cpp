#include <iostream>
#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/net/http/server/handler.h"
#include "thirdparty/monitor_sdk_common/net/http/server/server.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

using std::cout;
using std::endl;
namespace common {

    class TestHttpHandler : public HttpHandler {
        public:
            TestHttpHandler() {}
            ~TestHttpHandler() {}

            void HandleGet(const HttpRequest* req, HttpResponse* resp) {
                cout << "HandleGet: " << req->Body() << endl;
                resp->SetStatus(HttpResponse::Status_OK);
                std::string body("hello");
                resp->SetBody(body);
                resp->SetHeader("Content-Length", NumberToString(body.size()));
            }
    };
}  // namespace common

int main(int argc, char** argv) {
    FLAGS_alsologtostderr = true;
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    using namespace common;
    HttpServer server;
    HttpHandler* handler = new TestHttpHandler();
    server.RegisterHttpHandler("/test", handler);
    server.Bind(SocketAddressInet4("127.0.0.1", 8080));
    LOG(INFO) << "Listen on http://127.0.0.1:8080/";
    server.Start();
    server.Run();
    return 0;
}

