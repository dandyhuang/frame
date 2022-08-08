//
// Created by 吴婷 on 2019-12-10.
//
#include "thirdparty/monitor_sdk_common/libvep/entity/meta_Dto.h"
#include "thirdparty/monitor_sdk_common/libvep/config/constants.h"
#include "thirdparty/monitor_sdk_common/libvep/cache/cache_utils.h"
#include "client_report_service.h"

using namespace common;

ClientReportParam ClientReportService::generateReportInfo(long last_accumulate_updated_time) {
    return generateReportInfo(last_accumulate_updated_time, -1l, -1l);
}

ClientReportParam ClientReportService::generateReportInfo(long last_accumulate_updated_time, long last_watched_changing_time, long last_fetched_changing_time) {
    std::string app_name = VepLocalProperties::app_name;
    std::string scene_code = scene_Param_vo.scene_code;
    std::string version = VepLocalProperties::client_version;
    std::string ip = VepClientInfoUtils::getLocalIP();            //获取本地ip地址
    long request_timestamp = VepTimeUtils::GetCurrentTimeMills();  //获取当前时间

    std::vector<LabReportParam> lab_report_param_list;
    //从缓存中获取场景信息
    std::shared_ptr<VepApiSceneVo> scene_vo = std::make_shared<VepApiSceneVo>();
    if (LayerCacheUtils::get(scene_code, scene_vo)) {  //成功获取到缓存，继续取出各个实验上报信息
        lab_report_param_list = buildLabReportParamList(scene_vo->lab_name_list, scene_vo->lab_map);
    }

    //构建客户端上报
    ClientReportParam client_report_param(app_name, scene_code, version, ip,
            last_accumulate_updated_time, request_timestamp, last_watched_changing_time, last_fetched_changing_time,
            lab_report_param_list);
    return client_report_param;
}

/**
 * 构建实验列表信息
 * @param lab_name_list 实验列表名
 * @param lab_map 实验值对象map
 * @return
 */
std::vector<LabReportParam> ClientReportService::buildLabReportParamList(const std::vector<std::string> &lab_name_list,std::unordered_map<std::string, std::shared_ptr<VepApiLabVo>> lab_map) {
    std::vector<LabReportParam> param_list;
    if (lab_name_list.empty() > 0 && lab_map.size() > 0) {
        for (auto lab_name : lab_name_list){
            const LabDto &lab_dto = lab_map[lab_name]->lab;  //map取数
            int lab_id = lab_dto.id;
            long update_time = lab_dto.update_time;
            LabReportParam lab_report_param(lab_id, lab_name, update_time);
            param_list.push_back(lab_report_param);
        }
    }
    return param_list;
}