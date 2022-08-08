//
// Created by 吴婷 on 2019-12-09.
//
#include <memory>

#include "http_invoker.h"
#include "thirdparty/monitor_sdk_common/libvep/request/vep_req_param.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(VepHttpInvoker, buildUltimateUrl) {
    std::string url = "http://10.101.19.232:8080/vep/layer/layerConfig/ad/predict_1107";
    VepLayerReqParam layer_req("ad", "predict_1107", 1573089880267, -1l, 1573185645932);  //请求参数类
    std::shared_ptr<VepHttpInvoker> http_invoker_ptr = std::make_shared<VepHttpInvoker>();  //网络连接

    //std::string url2 = http_invoker_ptr->buildUltimateUrl(url, layer_req);  //构造连接url
    //EXPECT_EQ("http://10.101.19.232:8080/vep/layer/layerConfig/ad/predict_1107?reportParam=%7B%22bizType%22%3A%22ad%22%2C%22layerCode%22%3A%22predict_1107%22%2C%22lastLayerChangingTime%22%3A1573089880267%2C%22reportParam%22%3A%7B%22appName%22%3A%22%22%2C%22clientVersion%22%3A%221.2.0.20191013-SNAPSHOT%22%2C%22clientIp%22%3A%2210.13.213.138%22%2C%22lastLayerChangingTime%22%3A1573089880267%2C%22requestTimestamp%22%3A1573185943069%2C%22lastWatchedChangingTime%22%3A-1%2C%22lastFetchedChangingTime%22%3A1573185645932%7D%7D",
    //url2);

    std::map<std::string, std::string> param_map = {
            {"sceneCode", "scene_1106"},
            {"clientReport", "{\"appName\":\"vep-sdk\",\"clientIp\":\"10.101.100.109\",\"clientVersion\":\"1.2.0.20191013-SNAPSHOT\",\"labReportParams\":null,\"lastAccumulateUpdatedTime\":6292361198466,\"lastFetchedChangingTime\":-1,\"lastWatchedChangingTime\":-1,\"requestTimestamp\":1575983690764,\"sceneCode\":\"scene_1106\"}"}
    };
    std::string url3 = http_invoker_ptr->buildUltimateUrl("http://10.101.19.232:8080/vep/sceneConfig", param_map);  //构造连接url
    //EXPECT_EQ("", url3);   //查看构造的请求url是否正确
}

}