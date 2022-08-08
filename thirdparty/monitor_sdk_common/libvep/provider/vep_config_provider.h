//
// Created by 吴婷 on 2019-12-10.
//

#ifndef COMMON_LIBVEP_VEP_CONFIG_PROVIDER_H
#define COMMON_LIBVEP_VEP_CONFIG_PROVIDER_H

#include <atomic>
#include <memory>

#include "thirdparty/monitor_sdk_common/libvep/entity/scene_vo.h"
#include "thirdparty/monitor_sdk_common/libvep/cache/cache_utils.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/service/vep_config_service.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/service/client_report_service.h"
#include "thirdparty/monitor_sdk_common/libvep/result/resultVo.h"
#include "thirdparty/monitor_sdk_common/libvep/result/vep_lab_paramVo.h"
#include "thirdparty/monitor_sdk_common/libvep/vep_scheduled_executor.h"

namespace common {
/**
 * 场景配置提供器V1.0
 */
class VepConfigProvider {
public:
    VepConfigProvider();
    VepConfigProvider(std::shared_ptr <VepConfigService> vep_config_service_ptr,
                      std::shared_ptr <ClientReportService> client_report_service_ptr,
                      const VepSceneParamVo &scene_Param_vo);
    ~VepConfigProvider();

    void initialize();

    std::shared_ptr<VepApiSceneVo> getSceneVo();                 //获取场景值对象
    ResultVo <VepLabParamVo> abValues(const std::string &lab_name, const std::string &shunt, const std::string &tag, const std::string &traffic_type);   //返回查取结果
    void close();

    void initializeForGtest();      //gtest测试时，拉取一次

private:
    bool fetchRemoteConfig(const std::string &type_enum, const ClientReportParam &report_param); //尝试获取远端最新配置
    void initializeScheduled();     //初始化定时任务
    bool watchRemoteConfig();       //监听变更
    bool reportClientInfo();        //上报信息
    void generateVepLabParamVo(const std::string &shunt, const std::string &tag, const SceneDto &sceneDto, std::shared_ptr<VepApiLabVo> lab_vo, const std::string &traffic_allocation_type, VepLabParamVo *lab_param_vo);  //生成结果类
    std::string calculateHashShunt(const std::string &shunt, const LabDto &lab_dto);  //哈希号段
    inline void finish(int matched_version_id, std::shared_ptr<VepApiLabVo> lab_vo,  VepLabParamVo* lab_param_vo);
    bool getTarget(int id);
    std::unordered_map<std::string, std::set<int>> grabMapTagToVersion(std::shared_ptr<VepApiLabVo> vep_lab_vo);
    void rebuild(std::shared_ptr<VepApiLabVo> vep_lab_vo);
    bool validateUsingType(const std::string &layer_type, const std::string &using_type);     //判断流量分配类型是否匹配
    std::string resetFromUsingType(const std::string &shunt, const std::string &tag, const std::string &using_type); //获取分流方式

private:
    std::shared_ptr <VepConfigService> vep_config_service_ptr_;        //V1.0接口服务
    std::shared_ptr <ClientReportService> client_report_service_ptr_;  //生成客户端上报信息
    std::shared_ptr <VepScheduledExecutor> watch_scheduled_executor_ = nullptr;      //调度类: 定时任务线程池(用于长轮询监听是否发生变更)
    std::shared_ptr <VepScheduledExecutor> report_scheduled_executor_ = nullptr;      //调度类：定时拉取上报
    VepSceneParamVo scene_Param_vo_;               //场景参数

    std::atomic<bool> initialized_;
    //用于比对本地缓存与远程配置
    std::atomic<long> last_accumulate_updated_time_;
    std::atomic<long> last_watched_changing_time_;
    std::atomic<long> last_fetched_changing_time_;

    static int NOT_EXIST_;
};

}//namespace common
#endif //COMMON_LIBVEP_VEP_CONFIG_PROVIDER_H
