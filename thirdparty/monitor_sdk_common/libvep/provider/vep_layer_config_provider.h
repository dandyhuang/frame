//
// Created by 吴婷 on 2019-11-23.
//

#ifndef COMMON_LIBVEP_VEP_LAYER_CONFIG_PROVIDER_H
#define COMMON_LIBVEP_VEP_LAYER_CONFIG_PROVIDER_H

#include <atomic>
#include <memory>
#include <mutex>
#include <string>

#include "thirdparty/monitor_sdk_common/libvep/entity/layer_vo.h"
#include "thirdparty/monitor_sdk_common/libvep/entity/model_Dto.h"
#include "thirdparty/monitor_sdk_common/libvep/cache/cache_utils.h"
#include "thirdparty/monitor_sdk_common/libvep/cache/layer_cache_key_generator.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/service/layer/layer_config_service.h"
#include "thirdparty/monitor_sdk_common/libvep/result/resultVo.h"
#include "thirdparty/monitor_sdk_common/libvep/result/vep_lab_hitInfo.h"
#include "thirdparty/monitor_sdk_common/libvep/vep_scheduled_executor.h"

namespace common{
/**
 * V2.0实验层配置提供器
 */
class VepLayerConfigProvider{
public:
    //初始化
    VepLayerConfigProvider();
    VepLayerConfigProvider(const VepLayerClientDto &layer_param, std::shared_ptr<LayerConfigService> layer_config_service_ptr);
    ~VepLayerConfigProvider();
    void initialize();
    void initializeForGtest();
    void close();       //关闭
    void fetchAllLayerWithLaunchData();

    std::shared_ptr<VepApiLayerTrafficVo> getLayerTrafficVo(const std::string &layer_code);      //返回实验层流量信息
    std::shared_ptr<VepApiLaunchLayerVo> getLaunchLayerList(const std::string &layer_code);      //返回launch层流量信息
    ResultVo<VepLabHitInfo> abValues(const std::string &layer_code, const std::string &traffic_position_id, const std::string &shunt);   //返回匹配的实验信息
    //V2.2返回匹配的实验信息
    ResultVo<VepLabHitInfo> abValues(const std::string &layer_code, const std::string &traffic_position_id, const std::string &shunt, const std::string &tag, const std::string &traffic_type);

private:
    void registerScheduleTask();     //定时拉取任务
    void registerWatchTask();        //监听任务
    void handleWatching();      //获取监听信息

    void fetchAndStoreLaunchLayerInfo(const std::vector<std::string> &layer_code_vec, const std::string &type_enum);     //获取和存储Launch层信息
    void fetchAndStoreLayerInfo(const std::vector<std::string> &layer_code_vec, const std::string &type_enum);           //获取和存储实验层信息

    void buildFetchLayerTrafficTask(const std::string &layer_code); //线程池拉取实验层数据填充到map
    void buildFetchLaunchLayerDataTask(const std::string &layer_code); //线程池拉取launch层数据填充到map

    //V2.2根据匹配信息生成结果数据
    ResultVo<VepLabHitInfo> generateMatchedResultVo(const std::string &tag, const std::string &shunt, int matched_version_id
            , std::shared_ptr<ConversionLayerDto> conversion_layer_dto
            , const VepApiLayerVo& layer_vo);

    //V2.2
    void generateVepLabParamVo(const std::string &tag, const std::string &shunt, std::shared_ptr<LabVersionDto> version_dto
            , std::shared_ptr<LaunchLayerDto> launch_layer_dto
            , std::shared_ptr<VepApiLabWithPositionVo> lab_with_position_vo
            , const VepApiLayerVo& layer_vo
            , VepLabHitInfo* labHitInfo);

    std::string calculateHashShunt(const std::string &shunt, const VepApiLayerVo& layer_vo);  //生成哈希号段

    bool getTarget(int id); //判断id是否为空
    bool validateUsingType(const std::string &layer_type, const std::string &using_type);     //判断流量分配类型是否匹配
    std::string resetFromUsingType(const std::string &shunt, const std::string &tag, const std::string &using_type); //获取分流方式

        private:
    VepLayerClientDto layer_param_;                 //实验层实验类型参数
    std::shared_ptr<LayerConfigService> layer_config_service_ptr_;    //拉取实验层信息
    std::shared_ptr<VepScheduledExecutor> scheduled_executor_ = nullptr;          //调度类：定时调度
    std::shared_ptr<VepScheduledExecutor> watch_scheduled_executor_ = nullptr;    //调度类：监听

    std::unordered_map<std::string, std::shared_ptr<VepApiLayerTrafficVo> > layer_traffic_map_;  //{layerCode : Vo}
    std::unordered_map<std::string, std::shared_ptr<VepApiLaunchLayerVo> > launch_data_map_;     //{layerCode : Vo}

    static long INITIAL_DELAY_;
    static int NOT_EXIST_;

    std::atomic<bool> initialized_;          //可以使用std::atomic来代替volatile
    //用于比对本地缓存与远程配置
    std::atomic<long> last_layer_changing_time_;
    std::atomic<long> last_watched_changing_time_;
    std::atomic<long> last_fetched_changing_time_;
    ConversionLayerDto m_conversion_layer_dto;

};

}  //namespace common
#endif //COMMON_LIBVEP_VEP_LAYER_CONFIG_PROVIDER_H
