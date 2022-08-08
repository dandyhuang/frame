//
// Created by 吴婷 on 2019-12-04.
//

#include <memory>

#include "thirdparty/monitor_sdk_common/libvep/request/vep_req_param.h"
#include "thirdparty/monitor_sdk_common/libvep/util/converter_utils.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/service/vep_config_service.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/service/client_report_service.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/http/http_invoker.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/service/layer/layer_config_service.h"
#include "cache_utils.h"

#include "thirdparty/gtest/gtest.h"

namespace common {
TEST(LayerCacheUtils, putLayerAndGetLayer) {
    VepLayerReqParam layer_req("ad", "predict_1107", 1573089880267, -1l, 1573185645932);  //请求参数类
    std::shared_ptr<LayerConfigService> layer_config_service_ptr = std::make_shared<LayerConfigService>();
    std::shared_ptr<VepApiLayerTrafficVo> layer_traffic_vo;
    layer_traffic_vo = layer_config_service_ptr->getLayerTrafficVo(layer_req);  //获取实验层流量信息

    //转换：获取conversion_layer_dto数据
    std::shared_ptr<ConversionLayerDto> conversion_layer_dto;
    conversion_layer_dto = LayerConverter::convert(*layer_traffic_vo);

    LayerCacheUtils::putLayer("layer-ad-predict_1107", conversion_layer_dto);  //放入缓存

    std::shared_ptr<ConversionLayerDto> conversion_layer_dto2;
    LayerCacheUtils::getLayer("layer-ad-predict_1107", conversion_layer_dto2); //从缓存中取出


    VepApiLayerTrafficVo layer_traffic_vo2 = conversion_layer_dto2->layer_traffic_vo;  //验证缓存取出的结果
    EXPECT_EQ(29, layer_traffic_vo2.layer.id);
    EXPECT_EQ("ad", layer_traffic_vo2.layer.biz_line_name);
    EXPECT_EQ(1573070247422, (long)(layer_traffic_vo2.layer.create_time));

}

TEST(LayerCacheUtils, putLaunchLayerAndGetLaunchLayer) {
    VepLayerReqParam layer_req("ad", "predict_1107", 1573089880267, -1l, 1573185645932);  //请求参数类
    std::shared_ptr<LayerConfigService> layer_config_service_ptr = std::make_shared<LayerConfigService>();

    std::shared_ptr<VepApiLaunchLayerVo> launch_layer_vo;
    launch_layer_vo = layer_config_service_ptr->getLaunchLayerList(layer_req);  //获取launch层流量信息

    //转换：获取conversion_launch_layer_dto数据
    std::shared_ptr<ConversionLaunchLayerDto> conversion_launch_layer_dto;
    conversion_launch_layer_dto = LaunchLayerConverter::convert(*launch_layer_vo);

    LayerCacheUtils::putLaunchLayer("launch-ad-predict_1107", conversion_launch_layer_dto);  //放入缓存

    std::shared_ptr<ConversionLaunchLayerDto> conversion_launch_layer_dto2;
    LayerCacheUtils::getLaunchLayer("launch-ad-predict_1107", conversion_launch_layer_dto2); //从缓存中取出

    //验证取出的效果
    EXPECT_EQ("predict_1107", conversion_launch_layer_dto2->layer_code);
    std::vector<std::shared_ptr<LaunchLayerDto>> launch_layer_list = conversion_launch_layer_dto2->launch_layer_list;
    EXPECT_EQ(1, (int)launch_layer_list.size());
    EXPECT_EQ(44, launch_layer_list[0]->id);
    EXPECT_EQ("广告pctr模型01", launch_layer_list[0]->lab_variable_name);
}

//需要先有req, http, service
TEST(LayerCacheUtils, putAndGetScene) {
    VepSceneParamVo scene_Param_vo("scene_1106", "");         //场景参数
    ClientReportService client_report_service(scene_Param_vo);  //客户端信息生成器
    ClientReportParam client_report_param = client_report_service.generateReportInfo(6292361198466); //客户端信息
    std::shared_ptr<VepConfigService> vep_config_service_ptr = std::make_shared<VepConfigService>(scene_Param_vo); //服务提供器
    vep_config_service_ptr->initialize();
    std::shared_ptr<VepApiSceneVo> scene_vo = vep_config_service_ptr->getSceneVO(client_report_param);   //获取场景对象

    //LayerCacheUtils::put("scene_1106", scene_vo);  //放入

    //从缓存中取出
    //std::shared_ptr<VepApiSceneVo> scene_vo = std::make_shared<VepApiSceneVo>();
    //LayerCacheUtils::get("scene_1106", scene_vo);

    //scene
    EXPECT_EQ(38, scene_vo->scene.id);
    EXPECT_EQ("ad_1107", scene_vo->scene.module_code);
    //lab_name_list
    EXPECT_EQ(2, (int)(scene_vo->lab_name_list.size()));
    //last_accumulate_updated_time
    EXPECT_EQ(12596666333160, (long)(scene_vo->last_accumulate_updated_time));
    //lab_map
    EXPECT_EQ(2, (int)(scene_vo->lab_map.size()));
    std::shared_ptr<VepApiLabVo> &lab_vo = scene_vo->lab_map["lab-1106"];
    //1.lab
    EXPECT_EQ(54, lab_vo->lab.id);
    EXPECT_EQ("scene_1106", lab_vo->lab.scene_code);
    //2.versionMap
    EXPECT_EQ(2, (int)(lab_vo->version_map.size()));
    std::shared_ptr<LabVersionDto> &lab_version_dto = lab_vo->version_map[125];  //取出vector中的第一个
    EXPECT_EQ(125, lab_version_dto->id);

}

}
