//
// Created by 吴婷 on 2019-12-02.
//
#include <memory>

#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/service/vep_config_service.h"
#include "thirdparty/monitor_sdk_common/libvep/request/vep_req_param.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/http/http_invoker.h"
#include "layer_config_service.h"

#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(LayerConfigService, getLayerTrafficVo) {
    //LayerReportParam layer_report("", "1.2.0.20191013-SNAPSHOT", "10.13.213.138",
    //                             1573089880267, 1573185788238, -1l, 1573185645932);
    VepLayerReqParam layer_req("ad", "predict_1107", 1573089880267, -1l, 1573185645932);  //请求参数类
    std::shared_ptr <LayerConfigService> layer_config_service_ptr = std::make_shared<LayerConfigService>();
    layer_config_service_ptr->initialize();

    EXPECT_EQ("http://10.101.19.232:8080/vep/layer/layerConfig/ad/predict_1107", StringPrint("%s/vep/layer/layerConfig/%s/%s", VepConfigService::obtainApiServerUrl(), layer_req.biz_type, layer_req.layer_code));

    std::shared_ptr<VepApiLayerTrafficVo> layer_traffic_vo;
    layer_traffic_vo = layer_config_service_ptr->getLayerTrafficVo(layer_req);

    EXPECT_EQ(29, layer_traffic_vo->layer.id);
    EXPECT_EQ("ad", layer_traffic_vo->layer.biz_line_name);
    EXPECT_EQ(1573070247422, (long)(layer_traffic_vo->layer.create_time));

    EXPECT_EQ(5, (int)(layer_traffic_vo->lab_list.size()));

    EXPECT_EQ(3, (int)(layer_traffic_vo->lab_list[0]->version_list.size()));
    EXPECT_EQ("test_succeed", layer_traffic_vo->lab_list[0]->version_list[1]->integration_state);

    //测试场景
    VepLayerSceneReqParam layer_req2("ad", "predict_1107", 1573089880267, -1l, 1573185645932, "scene_1106");  //请求参数类
    std::shared_ptr<VepApiLayerTrafficVo> layer_traffic_vo2 = layer_config_service_ptr->getLayerTrafficVo(layer_req2);

    EXPECT_EQ(29, layer_traffic_vo2->layer.id);
    EXPECT_EQ("ad", layer_traffic_vo2->layer.biz_line_name);
    EXPECT_EQ(1573070247422, (long)(layer_traffic_vo2->layer.create_time));

    EXPECT_EQ(1, (int)(layer_traffic_vo2->lab_list.size()));

    EXPECT_EQ("scene_1106", layer_traffic_vo2->lab_list[0]->scene_code);
    EXPECT_EQ(2, (int)(layer_traffic_vo2->lab_list[0]->version_list.size()));
    EXPECT_EQ("广告pctr模型01", layer_traffic_vo2->lab_list[0]->version_list[1]->lab_variable_name);


}

TEST(LayerConfigService, hasChanged) {
    VepLayerReqParam layer_req("ad", "", 1573089880267, -1l, 1573185645932);  //请求参数类
    std::shared_ptr <LayerConfigService> layer_config_service_ptr = std::make_shared<LayerConfigService>();
    layer_config_service_ptr->initialize();

    EXPECT_EQ("http://10.101.19.232:8080/vep/v2/layer/watch", StringPrint("%s/vep/v2/layer/watch", VepConfigService::obtainApiServerUrl()));

    std::shared_ptr<VepApiLayerFetchVo> vepApiLayerFetchVo;
    vepApiLayerFetchVo = layer_config_service_ptr->hasChanged(layer_req);

    EXPECT_EQ("INCREMENT", vepApiLayerFetchVo->fetch_type);

    EXPECT_EQ(true, (int)(vepApiLayerFetchVo->change_list.size()) > 0);
    EXPECT_EQ(false, vepApiLayerFetchVo->fetch_type == "");
    //EXPECT_EQ(1575660670176, (long)(vepApiLayerFetchVo.change_list[0].create_time));
    //EXPECT_EQ("wt_test01", vepApiLayerFetchVo.change_list[0].layer_name);
}

TEST(LayerConfigService, getLaunchLayerList) {
    VepLayerReqParam layer_req("ad", "predict_1107", 1573089880267, -1l, 1573185645932);  //请求参数类
    std::shared_ptr <LayerConfigService> layer_config_service_ptr = std::make_shared<LayerConfigService>();
    layer_config_service_ptr->initialize();

    EXPECT_EQ("http://10.101.19.232:8080/vep/layer/launchLayerConfig/ad/predict_1107", StringPrint("%s/vep/layer/launchLayerConfig/%s/%s", VepConfigService::obtainApiServerUrl(), layer_req.biz_type, layer_req.layer_code));

    std::shared_ptr<VepApiLaunchLayerVo> launch_layer_vo;
    launch_layer_vo = layer_config_service_ptr->getLaunchLayerList(layer_req);

    EXPECT_EQ("predict_1107", launch_layer_vo->layer_code);
    EXPECT_EQ(1, (int)(launch_layer_vo->launch_layer_list.size()));
    //EXPECT_EQ(1575660670176, (long)(launch_layer_vo.last_layer_changing_time));

    EXPECT_EQ("{\"pctr\":\"4.1\",\"ds\":\"4.1\"}", launch_layer_vo->launch_layer_list[0]->var_param);

}

}
