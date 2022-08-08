//
// Created by 吴婷 on 2019-12-03.
//
#include <memory>
#include <vector>

#include "converter_utils.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/service/vep_config_service.h"
#include "thirdparty/monitor_sdk_common/libvep/request/vep_req_param.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/http/http_invoker.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/service/layer/layer_config_service.h"

#include "thirdparty/gtest/gtest.h"

namespace common {
TEST(LayerConverter, convert) {
    VepLayerReqParam layer_req("ad", "predict_1107", 1573089880267, -1l, 1573185645932);  //请求参数类
    std::shared_ptr<LayerConfigService> layer_config_service_ptr = std::make_shared<LayerConfigService>();

    std::shared_ptr<VepApiLayerTrafficVo> layer_traffic_vo;
    layer_traffic_vo = layer_config_service_ptr->getLayerTrafficVo(layer_req);  //获取实验层流量信息

    EXPECT_EQ(29, layer_traffic_vo->layer.id);
    EXPECT_EQ("ad", layer_traffic_vo->layer.biz_line_name);
    EXPECT_EQ(1573070247422, (long)(layer_traffic_vo->layer.create_time));

    //转换
    std::shared_ptr<ConversionLayerDto> conversion_layer_dto;
    conversion_layer_dto = LayerConverter::convert(*layer_traffic_vo);

    VepApiLayerTrafficVo layer_traffic_vo2 = conversion_layer_dto->layer_traffic_vo;
    EXPECT_EQ(29, layer_traffic_vo2.layer.id);
    EXPECT_EQ("ad", layer_traffic_vo2.layer.biz_line_name);
    EXPECT_EQ(1573070247422, (long)(layer_traffic_vo2.layer.create_time));
}

TEST(LaunchLayerConverter, convert) {
    VepLayerReqParam layer_req("ad", "predict_1107", 1573089880267, -1l, 1573185645932);  //请求参数类
    std::shared_ptr<LayerConfigService> layer_config_service_ptr = std::make_shared<LayerConfigService>();

    std::shared_ptr<VepApiLaunchLayerVo> launch_layer_vo;
    launch_layer_vo = layer_config_service_ptr->getLaunchLayerList(layer_req);  //获取launch层流量信息

    EXPECT_EQ("predict_1107", launch_layer_vo->layer_code);
    EXPECT_EQ(1, (int)(launch_layer_vo->launch_layer_list.size()));
    //EXPECT_EQ(1575265547484, (long)(launch_layer_vo.last_layer_changing_time));
    EXPECT_EQ("{\"pctr\":\"4.1\",\"ds\":\"4.1\"}", launch_layer_vo->launch_layer_list[0]->var_param);

    //转换
    std::shared_ptr<ConversionLaunchLayerDto> conversion_launch_layer_dto;
    conversion_launch_layer_dto = LaunchLayerConverter::convert(*launch_layer_vo);
    EXPECT_EQ("predict_1107", conversion_launch_layer_dto->layer_code);

    std::vector<std::shared_ptr<LaunchLayerDto>> launch_layer_list = conversion_launch_layer_dto->launch_layer_list;
    EXPECT_EQ(1, (int)launch_layer_list.size());
    EXPECT_EQ(44, launch_layer_list[0]->id);
    EXPECT_EQ("广告pctr模型01", launch_layer_list[0]->lab_variable_name);
}

}
