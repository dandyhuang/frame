//
// Created by 吴婷 on 2019-12-11.
//
#include <memory>

#include "thirdparty/monitor_sdk_common/libvep/util/converter_utils.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/service/client_report_service.h"
#include "vep_config_service.h"

#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(VepConfigService, getSceneVO) {
    VepSceneParamVo scene_Param_vo("scene_1106", "");         //场景参数
    ClientReportService client_report_service(scene_Param_vo);  //客户端信息生成器
    ClientReportParam client_report_param = client_report_service.generateReportInfo(6292361198466); //客户端信息
    std::shared_ptr<VepConfigService> vep_config_service_ptr = std::make_shared<VepConfigService>(scene_Param_vo); //服务提供器
    vep_config_service_ptr->initialize();
    std::shared_ptr<VepApiSceneVo> scene_vo = vep_config_service_ptr->getSceneVO(client_report_param);   //获取场景对象

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

TEST(VepConfigService, hasChanged) {
    VepSceneParamVo scene_Param_vo("scene_1106", "");         //场景参数
    ClientReportService client_report_service(scene_Param_vo);  //客户端信息生成器
    ClientReportParam client_report_param = client_report_service.generateReportInfo(6292361198466); //客户端信息
    std::shared_ptr<VepConfigService> vep_config_service_ptr = std::make_shared<VepConfigService>(scene_Param_vo); //服务提供器
    vep_config_service_ptr->initialize();

    VepSceneReqParam scene_req_param("scene_1106", 6292361198466);    //请求信息
    bool result = vep_config_service_ptr->hasChanged(scene_req_param, client_report_param); //是否改变的结果

    EXPECT_EQ(true, result);
}

TEST(VepConfigService, reportClientInfo) {
    VepSceneParamVo scene_Param_vo("scene_1106", "");         //场景参数
    ClientReportService client_report_service(scene_Param_vo);  //客户端信息生成器
    ClientReportParam client_report_param = client_report_service.generateReportInfo(6292361198466); //客户端信息
    std::shared_ptr<VepConfigService> vep_config_service_ptr = std::make_shared<VepConfigService>(scene_Param_vo); //服务提供器
    vep_config_service_ptr->initialize();

    bool result = vep_config_service_ptr->reportClientInfo(client_report_param);
    EXPECT_EQ(true, result);
}

}
