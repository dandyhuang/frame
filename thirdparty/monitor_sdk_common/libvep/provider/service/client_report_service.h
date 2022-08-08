//
// Created by 吴婷 on 2019-12-10.
//

#ifndef COMMON_LIBVEP_CLIENT_REPORT_SERVICE_H
#define COMMON_LIBVEP_CLIENT_REPORT_SERVICE_H

#include <map>
#include <vector>
#include <memory>

#include "thirdparty/monitor_sdk_common/base/unordered_map.h"
#include "thirdparty/monitor_sdk_common/libvep/entity/scene_vo.h"
#include "thirdparty/monitor_sdk_common/libvep/request/client_report_param.h"

namespace common {
/**
 * 客户端信息生成 V1.0
 */
class ClientReportService {
public:
    ClientReportService(const VepSceneParamVo &scene_Param_vo):scene_Param_vo(scene_Param_vo) {}

    ClientReportParam generateReportInfo(long last_accumulate_updated_time);
    ClientReportParam generateReportInfo(long last_accumulate_updated_time, long last_watched_changing_time, long last_fetched_changing_time);

private:
    std::vector<LabReportParam> buildLabReportParamList(const std::vector<std::string> &lab_name_list, std::unordered_map<std::string, std::shared_ptr<VepApiLabVo>> lab_map);

private:
    VepSceneParamVo scene_Param_vo;  //用户传入场景参数
};

}//namespace common
#endif //COMMON_LIBVEP_CLIENT_REPORT_SERVICE_H
