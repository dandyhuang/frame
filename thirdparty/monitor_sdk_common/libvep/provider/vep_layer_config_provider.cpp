//
// Created by 吴婷 on 2019-11-23.
//
#include <map>
#include <vector>
#include <exception>
#include <iostream>

#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/libvep/entity/model_Dto.h"
#include "thirdparty/monitor_sdk_common/libvep/config/constants.h"
#include "thirdparty/monitor_sdk_common/libvep/config/dynamic_client_config.h"
#include "thirdparty/monitor_sdk_common/libvep/request/vep_req_param.h"
#include "thirdparty/monitor_sdk_common/libvep/util/log_utils.h"
#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"
#include "thirdparty/monitor_sdk_common/libvep/util/deserialize_utils.h"
#include "thirdparty/monitor_sdk_common/libvep/util/converter_utils.h"
#include "vep_layer_config_provider.h"

using namespace common;

long VepLayerConfigProvider::INITIAL_DELAY_ = 10000L;
int VepLayerConfigProvider::NOT_EXIST_ = -1;

VepLayerConfigProvider::VepLayerConfigProvider(const VepLayerClientDto &layer_param, std::shared_ptr<LayerConfigService> layer_config_service_ptr):
        layer_param_(layer_param), layer_config_service_ptr_(layer_config_service_ptr) {
    initialized_ = false;
    last_layer_changing_time_ = -1l;
    last_watched_changing_time_ = -1l;
    last_fetched_changing_time_ = -1l;
}

VepLayerConfigProvider::~VepLayerConfigProvider() {
    close();
}

void VepLayerConfigProvider::initialize() {
    if (!initialized_) {
        //初始化调度类
        scheduled_executor_ = std::make_shared<VepScheduledExecutor>(3);
        watch_scheduled_executor_ = std::make_shared<VepScheduledExecutor>(1);

        //启动service
        layer_config_service_ptr_->initialize();

        // 1.拉取全部的实验层+Launch信息
        fetchAllLayerWithLaunchData();

        // 2.注册定时任务、长轮询任务
        registerScheduleTask();
        registerWatchTask();

        //定时拉取配置
        scheduled_executor_->ScheduleWithFixedDelay([this] (const bool *terminated) {
            LogUtils::LogInfo(StringPrint("VepLayerConfigProvider配置中心定时拉取 >> %d", ConfigConstants::SCHEDULED_FETCH_CONFIG_INTERVAL_MILLISECONDS));
            if((*terminated)) return;
            DynamicClientConfig::Instance()->DynamicClientConfigScheduled();
        }, ConfigConstants::SCHEDULED_FETCH_CONFIG_INTERVAL_MILLISECONDS, ConfigConstants::SCHEDULED_FETCH_CONFIG_INTERVAL_MILLISECONDS);

        initialized_ = true;
    }
}

/**
 * 为了gtest测试，不开启调度初始化
 */
void VepLayerConfigProvider::initializeForGtest() {
    fetchAllLayerWithLaunchData();
}

/**
 * 拉取实验层数据填充到map
 */
void VepLayerConfigProvider::buildFetchLayerTrafficTask(const std::string &layer_code) {
    long start = VepTimeUtils::GetCurrentTimeMills();  //开始拉取时间
    LogUtils::LogInfo(StringPrint("%s: 开始拉取layer层", "buildFetchLayerTrafficTask"));

    std::shared_ptr<VepApiLayerTrafficVo> traffic_vo = getLayerTrafficVo(layer_code); //拉取实验层数据

    if  (traffic_vo->last_layer_changing_time != -1l) {                   //如http连接拉取数据成功
        layer_traffic_map_.insert({layer_code, traffic_vo});      //填充map
        LogUtils::LogInfo(StringPrint("%s: [%s] 插入layer_traffic_map_成功", "buildFetchLayerTrafficTask", layer_code));
    }

    long end = VepTimeUtils::GetCurrentTimeMills();  //结束拉取时间
    LogUtils::LogInfo(StringPrint("%s: 本次拉取layer层成功，用时：%ld 毫秒", "buildFetchLayerTrafficTask", end-start));
}

/**
 * 拉取launch层数据填充到map
 */
void VepLayerConfigProvider::buildFetchLaunchLayerDataTask(const std::string &layer_code) {
    long start = VepTimeUtils::GetCurrentTimeMills();  //开始拉取时间
    LogUtils::LogInfo(StringPrint("%s: 开始拉取launch层", "buildFetchLaunchLayerDataTask"));

    std::shared_ptr<VepApiLaunchLayerVo> launch_layer_vo = getLaunchLayerList(layer_code); //拉取launch层数据

    if (launch_layer_vo->last_layer_changing_time != -1l) { //如http连接拉取数据成功
        launch_data_map_.insert({layer_code, launch_layer_vo});      //填充map
        LogUtils::LogInfo(StringPrint("%s: [%s] 插入launch_data_map_成功", "buildFetchLaunchLayerDataTask", layer_code));
    }

    long end = VepTimeUtils::GetCurrentTimeMills();  //结束拉取时间
    LogUtils::LogInfo(StringPrint("%s: 本次拉取launch层成功，用时：%ld 毫秒", "buildFetchLaunchLayerDataTask", end-start));
}

/**
 * 提取全部的Layer层信息
 */
void VepLayerConfigProvider::fetchAllLayerWithLaunchData() {
    try {
        // 1.遍历layerCode集合，依次去获取到最新的实验层数据
        for (auto layer_code : layer_param_.layer_code_set) {
            buildFetchLayerTrafficTask(layer_code);  //拉取实验层数据填充到map
        }
        LogUtils::LogInfo(StringPrint("layer_traffic_map size [%d]", layer_traffic_map_.size()));

        //2.遍历layerCode集合，依次去获取到最新的Launch层信息
        for (auto layer_code : layer_param_.layer_code_set) {
            buildFetchLaunchLayerDataTask(layer_code);
        }
        LogUtils::LogInfo(StringPrint("launch_data_map size [%d]", launch_data_map_.size()));

        long max_last_layer_changing_time = last_layer_changing_time_;

        //3.获取成功，解析数据，写入到内存，建立映射关系【直接将实验层和launch层的数据读入缓存，然后在用的时候取出】
        for (const auto &item : layer_traffic_map_) {
            std::shared_ptr<VepApiLayerTrafficVo> traffic_vo = item.second;
            std::shared_ptr<ConversionLayerDto> conversion_layer_dto = LayerConverter::convert(*traffic_vo);  //建立映射关系
            if (conversion_layer_dto->layer_traffic_vo.last_layer_changing_time != -1L) {  //非空类
                std::string cache_key;
                LayerCacheKeyGenerator::generateLayerCacheKey(layer_param_, item.first, &cache_key);  //得到保存的key
                LayerCacheUtils::putLayer(cache_key, conversion_layer_dto);  //放入缓存
                LogUtils::LogInfo(StringPrint("[%s] put cache", cache_key));

                if(traffic_vo->last_layer_changing_time > max_last_layer_changing_time){
                    max_last_layer_changing_time = traffic_vo->last_layer_changing_time;
                }
            }
        }

        for (const auto &item : launch_data_map_) {
            std::shared_ptr<VepApiLaunchLayerVo> launch_layer_vo = item.second;
            std::shared_ptr<ConversionLaunchLayerDto> launch_layer_dto = LaunchLayerConverter::convert(*launch_layer_vo); //建立映射关系
            if(launch_layer_dto->layer_code != "" && launch_layer_dto->launch_layer_list.size() != 0){  //非空类
                std::string cache_key;
                LayerCacheKeyGenerator::generateLaunchLayerCacheKey(layer_param_, item.first, &cache_key);  //得到保存的key
                LayerCacheUtils::putLaunchLayer(cache_key, launch_layer_dto);  //放入缓存
                LogUtils::LogInfo(StringPrint("[%s] put cache", cache_key));

                if(launch_layer_vo->last_layer_changing_time > max_last_layer_changing_time){
                    max_last_layer_changing_time = launch_layer_vo->last_layer_changing_time;
                }
            }
        }
        // 写最大的变更时间
        this->last_layer_changing_time_ = max_last_layer_changing_time;
        this->last_fetched_changing_time_ = VepTimeUtils::GetCurrentTimeMills();
        LogUtils::LogInfo(StringPrint("last_layer_changing_time_ : %s", IntegerToString(last_layer_changing_time_)));

    } catch (std::exception& e){
        LogUtils::LogError(StringPrint("DefaultVepLayerConfigProvider fetch data error, layer_param:%s", layer_param_.ToString()));
    }
}

/**
 * 定时拉取任务
 */
void VepLayerConfigProvider::registerScheduleTask() {
    std::vector<std::string> layer_code_vec = layer_param_.layer_code_set;
    scheduled_executor_->ScheduleWithFixedDelay([this, layer_code_vec] (const bool *terminated) {
        LogUtils::LogInfo(StringPrint("%s: 开始调度", "registerScheduleTask"));
        if((*terminated)) return;
        //拉取和缓存：launch层与实验层
        fetchAndStoreLaunchLayerInfo(layer_code_vec, FetchRemoteConfigTypeEnum::LONG_POLLING);
        fetchAndStoreLayerInfo(layer_code_vec, FetchRemoteConfigTypeEnum::LONG_POLLING);
    }, INITIAL_DELAY_, DynamicClientConfig::getScheduledFetchInterval());

}

/**
 * 监听任务
 */
void VepLayerConfigProvider::registerWatchTask() {
    watch_scheduled_executor_->ScheduleWithFixedDelay([this] (const bool *terminated) {
        LogUtils::LogInfo(StringPrint("%s: 开始调度", "registerWatchTask"));
        if((*terminated)) return;
        //拉取和缓存：launch层与实验层
        handleWatching();
    }, INITIAL_DELAY_, DynamicClientConfig::getScheduledFetchInterval());
}

/**
 * 获取监听信息
 */
void VepLayerConfigProvider::handleWatching() {
    LogUtils::LogInfo(StringPrint(">>>>%s<<<<", "VepLayerConfigProvider::handleWatching"));
    std::shared_ptr<VepApiLayerFetchVo> fetch_vo = std::make_shared<VepApiLayerFetchVo>(); //实验层监控拉取信息
    //第三方业务类型+关联实验层
    if (LayerUsingTypeEnum::THIRD_PARTY_WITH_LAYER == layer_param_.layer_using_type) {
        LogUtils::LogInfo(StringPrint(">>>>%s<<<<", "VepLayerReqParam"));
        VepLayerReqParam layer_req(layer_param_.position_type, "", last_layer_changing_time_, last_watched_changing_time_, last_fetched_changing_time_);  //请求参数类
        fetch_vo = layer_config_service_ptr_->hasChanged(layer_req);
    } else { //场景+关联实验层
        LogUtils::LogInfo(StringPrint(">>>>%s<<<<", "VepLayerSceneReqParam"));
        VepLayerSceneReqParam scene_req(layer_param_.position_type, "",
                last_layer_changing_time_, last_watched_changing_time_, last_fetched_changing_time_,
                layer_param_.scene_code);  //请求参数类
        fetch_vo = layer_config_service_ptr_->hasChanged(scene_req);
    }
    //1.判断是否有更新数据
    if (fetch_vo->change_list.size() == 0 && fetch_vo->fetch_type == "") {
        LogUtils::LogInfo(StringPrint("%s: 定时获取监听信息返回:无更新或者异常<<", "handleWatching"));
        return; //异常直接退出
    }

    this->last_watched_changing_time_ = VepTimeUtils::GetCurrentTimeMills();
    //2.如果有更新数据，就进行更新
    if (LayerFetchTypeEnum::ALL == fetch_vo->fetch_type) {
        LogUtils::LogInfo(StringPrint("%s: 开始全量拉取>>", "handleWatching"));
        //全量拉取
        fetchAllLayerWithLaunchData();
    } else {
        LogUtils::LogInfo(StringPrint("%s: 开始增量拉取>>", "handleWatching"));
        //增量拉取
        std::vector<std::shared_ptr<RelatedLayerRelevanceChangeDto>> change_list = fetch_vo->change_list; //实验层关联信息变更信息
        if (change_list.size() == 0) {
            std::vector<std::string> fetch_layer_codes = layer_param_.layer_code_set;  //当前实验层的code
            //统计是需要拉取Launch信息，还是需要拉取实验层信息
            std::vector<std::string> launch_change_set;   //待更新的launchCode
            std::vector<std::string> layer_change_set;    //待更新的layerCode
            for (auto relevance_change_dto : change_list) {
                //在layerCodeSet集合里面的，才去更新
                std::vector<std::string>::iterator it;
                it = std::find(fetch_layer_codes.begin(), fetch_layer_codes.end(), relevance_change_dto->layer_code);
                if (it != fetch_layer_codes.end()) {
                    if (LayerRelevanceChangeTypeEnum::CHANGE_OF_LAUNCH == relevance_change_dto->change_type) { //launch层更新
                        launch_change_set.push_back(relevance_change_dto->layer_code);
                    } else {
                        layer_change_set.push_back(relevance_change_dto->layer_code);
                    }
                }
            }
            //更新launch层
            if (!launch_change_set.empty()) {
                fetchAndStoreLaunchLayerInfo(launch_change_set, FetchRemoteConfigTypeEnum::LONG_POLLING);
            }
            //更新实验层
            if (!layer_change_set.empty()) {
                fetchAndStoreLayerInfo(layer_change_set, FetchRemoteConfigTypeEnum::LONG_POLLING);
            }
        }
        this->last_fetched_changing_time_ = VepTimeUtils::GetCurrentTimeMills();
    }
    LogUtils::LogInfo(StringPrint("%s: 定时获取监听信息完成>>", "handleWatching"));
}

/**
 * 获取和存储实验层信息
 * @param layer_code
 * @param type_enum
 */
void VepLayerConfigProvider::fetchAndStoreLayerInfo(const std::vector<std::string> &layer_code_vec, const std::string &type_enum) {
    for (auto layer_code : layer_code_vec) {
        std::shared_ptr<VepApiLayerTrafficVo> traffic_vo = getLayerTrafficVo(layer_code);
        std::shared_ptr<ConversionLayerDto> conversion_layer_dto = LayerConverter::convert(*traffic_vo);  //建立映射关系
        if (conversion_layer_dto->layer_traffic_vo.last_layer_changing_time != -1L) {  //非空类
            //定时拉取/改变时间在当前时间之后，就更新缓存
            if (FetchRemoteConfigTypeEnum::SCHEDULE == type_enum || traffic_vo->last_layer_changing_time > last_layer_changing_time_) {
                std::string cache_key;
                LayerCacheKeyGenerator::generateLayerCacheKey(layer_param_, layer_code, &cache_key);  //得到保存的key
                LayerCacheUtils::putLayer(cache_key, conversion_layer_dto);  //放入缓存
                last_layer_changing_time_ = traffic_vo->last_layer_changing_time;
            }
            LogUtils::LogInfo(StringPrint("[%s] 获取和存储实验层信息成功>> [%s]", "fetchAndStoreLayerInfo", layer_code));
        } else {
            LogUtils::LogInfo(StringPrint("[%s] 获取和存储实验层信息失败<< [%s]", "fetchAndStoreLayerInfo", layer_code));
        }
    }
}

/**
 * 获取和存储launch层信息
 * @param layer_code
 * @param type_enum
 */
void VepLayerConfigProvider::fetchAndStoreLaunchLayerInfo(const std::vector<std::string> &layer_code_vec, const std::string &type_enum) {
    for (auto layer_code : layer_code_vec) {
        std::shared_ptr<VepApiLaunchLayerVo> launch_layer_vo = getLaunchLayerList(layer_code);
        std::shared_ptr<ConversionLaunchLayerDto> launch_layer_dto = LaunchLayerConverter::convert(*launch_layer_vo);

        if (launch_layer_dto->launch_layer_list.size() > 0) {  //非空类
            //定时拉取/改变时间在当前时间之后，就更新缓存
            if (FetchRemoteConfigTypeEnum::SCHEDULE == type_enum || launch_layer_vo->last_layer_changing_time > last_layer_changing_time_) {
                std::string cache_key;
                LayerCacheKeyGenerator::generateLaunchLayerCacheKey(layer_param_, layer_code, &cache_key);  //得到保存的key
                LayerCacheUtils::putLaunchLayer(cache_key, launch_layer_dto);  //放入缓存
                LogUtils::LogInfo(StringPrint("[%s] put cache", cache_key));

                last_layer_changing_time_ = launch_layer_vo->last_layer_changing_time;
            }
            LogUtils::LogInfo(StringPrint("[%s] 获取和存储launch层信息成功>> [%s]", "fetchAndStoreLaunchLayerInfo", layer_code));
        } else {
            LogUtils::LogInfo(StringPrint("[%s] 获取和存储launch层信息失败<< [%s]", "fetchAndStoreLaunchLayerInfo", layer_code));
        }
    }
}

/**
 * 返回launch层流量信息
 * @param layer_code
 * @return
 */
std::shared_ptr<VepApiLaunchLayerVo> VepLayerConfigProvider::getLaunchLayerList(const std::string &layer_code) {
    //第三方业务类型+关联实验层
    if (LayerUsingTypeEnum::THIRD_PARTY_WITH_LAYER == layer_param_.layer_using_type) {
        VepLayerReqParam layer_req(layer_param_.position_type, layer_code, last_layer_changing_time_, last_watched_changing_time_, last_fetched_changing_time_);  //请求参数类
        std::shared_ptr<VepApiLaunchLayerVo> launch_layer_vo = layer_config_service_ptr_->getLaunchLayerList(layer_req);
        if (launch_layer_vo->last_layer_changing_time == -1l) {
            LogUtils::LogInfo(StringPrint("%s: 获取launch层数据失败", "getLaunchLayerList"));
        }
        return launch_layer_vo;
    }
    //场景+实验层
    VepLayerSceneReqParam scene_req(layer_param_.position_type, layer_code,
                                   last_layer_changing_time_, last_watched_changing_time_, last_fetched_changing_time_,
                                   layer_param_.scene_code);
    std::shared_ptr<VepApiLaunchLayerVo> launch_layer_vo = layer_config_service_ptr_->getLaunchLayerList(scene_req);
    if (-1l == launch_layer_vo->last_layer_changing_time) {
        LogUtils::LogInfo(StringPrint("%s: 获取launch层[场景]数据失败", "getLaunchLayerList"));
    }
    return launch_layer_vo;
}

/**
 * 返回实验层流量信息
 * @param layer_code
 * @return
 */
std::shared_ptr<VepApiLayerTrafficVo> VepLayerConfigProvider::getLayerTrafficVo(const std::string &layer_code) {
    //第三方业务类型+关联实验层
    if (LayerUsingTypeEnum::THIRD_PARTY_WITH_LAYER == layer_param_.layer_using_type) {
        VepLayerReqParam layer_req(layer_param_.position_type, layer_code, last_layer_changing_time_, last_watched_changing_time_, last_fetched_changing_time_);  //请求参数类
        std::shared_ptr<VepApiLayerTrafficVo> layer_traffic_vo = layer_config_service_ptr_->getLayerTrafficVo(layer_req);
        if (-1l == layer_traffic_vo->last_layer_changing_time) {
            LogUtils::LogInfo(StringPrint("%s: 获取实验层数据失败", "getLayerTrafficVo"));
        }
        return layer_traffic_vo;
    }
    //场景+实验层
    VepLayerSceneReqParam scene_req(layer_param_.position_type, layer_code,
            last_layer_changing_time_, last_watched_changing_time_, last_fetched_changing_time_,
            layer_param_.scene_code);
    std::shared_ptr<VepApiLayerTrafficVo> layer_traffic_vo = layer_config_service_ptr_->getLayerTrafficVo(scene_req);
    if (-1l == layer_traffic_vo->last_layer_changing_time) {
        LogUtils::LogInfo(StringPrint("%s: 获取实验层[场景]数据失败", "getLayerTrafficVo"));
    }
    return layer_traffic_vo;
}


/**
* 返回匹配的实验信息
* @param layer_code 实验层code
* @param traffic_position_id 流量位类型
* @param shunt 号段
* @param tag 圈定字段
* @param traffic_type 流量分配类型枚举类
* @return
*/
ResultVo<VepLabHitInfo> VepLayerConfigProvider::abValues(const std::string &layer_code, const std::string &traffic_position_id, const std::string &shunt, const std::string &tag, const std::string &traffic_type) {
    //参数判断
    if (!VepStringUtils::IsStringNotblank(layer_code)) { //为空
        ResultVo<VepLabHitInfo> ret(IntegerToString(ResultConstants::LAYER_NOT_FOUND.first), ResultConstants::LAYER_NOT_FOUND.second, false);
        return ret;
    }

    //缓存取出数据
    std::string cache_layer_key;
    LayerCacheKeyGenerator::generateLayerCacheKey(layer_param_, layer_code, &cache_layer_key);

    std::shared_ptr<ConversionLayerDto> conversion_layer_dto = std::make_shared<ConversionLayerDto>();
    LayerCacheUtils::getLayer(cache_layer_key, conversion_layer_dto); //从缓存中取出
    LogUtils::LogInfo(StringPrint("get [%s] from cache", cache_layer_key));

    if (conversion_layer_dto->layer_traffic_vo.last_layer_changing_time == -1) {  //空类
        LogUtils::LogInfo(StringPrint("%s: 缓存取出为空", "abValues"));
        ResultVo<VepLabHitInfo> ret(IntegerToString(ResultConstants::LAYER_NOT_FOUND.first), ResultConstants::LAYER_NOT_FOUND.second, false);
        return ret;
    }
    const VepApiLayerVo& layer_vo = conversion_layer_dto->layer_traffic_vo.layer;   //得到实验层信息
    std::string traffic_allocation_type = VepParseTrafficRuleUtils::grabTrafficRule(layer_vo);  //获取分流方式
    if (!validateUsingType(traffic_allocation_type, traffic_type)) {
        LogUtils::LogInfo(StringPrint("%s: 使用api与实验配置方式不匹配!", "abValues"));
        ResultVo<VepLabHitInfo> ret(IntegerToString(ResultConstants::USING_API_NOT_CONSIST_WITH_LAB_CONFIG.first), ResultConstants::USING_API_NOT_CONSIST_WITH_LAB_CONFIG.second, false);
        return ret;
    }
    if (0 == layer_vo.enable) {   //不启用
        LogUtils::LogInfo(StringPrint("%s: 不启用enable = 0", "abValues"));
        ResultVo<VepLabHitInfo> ret(IntegerToString(ResultConstants::LAYER_HAS_DISABLED.first), ResultConstants::LAYER_HAS_DISABLED.second, false);
        return ret;
    }
    if (VepStringUtils::IsBlank(shunt) && TrafficAllocationTypeEnum::hasSeg(traffic_allocation_type)) {
        LogUtils::LogInfo(StringPrint("%s: 号段分流，但号段为空，失败", "abValues"));
        ResultVo<VepLabHitInfo> ret(IntegerToString(ResultConstants::EMPTY_SHUNT.first), ResultConstants::EMPTY_SHUNT.second, false);
        return ret;
    }
    if (VepStringUtils::IsBlank(tag)
            && TrafficAllocationTypeEnum::hasTag(traffic_allocation_type)
            && !TrafficAllocationTypeEnum::hasSeg(traffic_allocation_type)) {
        LogUtils::LogInfo(StringPrint("%s: 仅圈定字段分流，但圈定字段为空，失败", "abValues"));
        ResultVo<VepLabHitInfo> ret(IntegerToString(ResultConstants::EMPTY_TAG.first), ResultConstants::EMPTY_TAG.second, false);
        return ret;
    }

    //1.匹配是否命中白名单
    //取出白名单
    const std::unordered_map<std::string, std::unordered_map<std::string, int>> &traffic_whitelist_mappings = conversion_layer_dto->traffic_whitelist_mappings;
    //先按流量位找
    auto iter_list_map = traffic_whitelist_mappings.find(traffic_position_id);
    if (iter_list_map != traffic_whitelist_mappings.end()) { //包含，就继续
        const std::unordered_map<std::string, int> &whitelist_part = iter_list_map->second;
        //再按号段找
        auto iter = whitelist_part.find(shunt);
        if (iter != whitelist_part.end()) {  //包含，命中白名单
            int matched_version_id = iter->second;
            LogUtils::LogInfo(StringPrint("%s: 命中白名单", "abValues"));
            return generateMatchedResultVo(tag, shunt, matched_version_id, conversion_layer_dto, layer_vo);  //生成匹配结果信息
        }
    }

    //2.计算号段，匹配是否存在号段映射到实验组
    int matched_version_id = -1;
    //2.1 分流字段：号段查找
    const std::unordered_map<std::string, std::unordered_map<std::string, int> > &traffic_segment_mappings = conversion_layer_dto->traffic_segment_mappings;
    std::string reset_traffic_type = resetFromUsingType(shunt, tag, traffic_type);   //分流方式
    //(1)判断分流方式
    //分流：号段分流
    if (TrafficAllocationTypeEnum::hasSeg(reset_traffic_type)) {
        //再按号段找
        std::string hash_shunt = calculateHashShunt(shunt, layer_vo);  //计算哈希号段：一定有3位
        std::string real_match_segment = VepStringUtils::leftPad(hash_shunt, ConfigConstants::DEFAULT_HASH_SEGMENT_LENGTH, "0");
        //先按流量位找
        auto iter_list_map2 = traffic_segment_mappings.find(traffic_position_id);
        if (iter_list_map2 != traffic_segment_mappings.end()) {  //包含，就继续
            const std::unordered_map<std::string, int> &segment_part = iter_list_map2->second;
            auto iter_seg = segment_part.find(real_match_segment);
            if(iter_seg != segment_part.end()){  //包含，命中号段名单
                matched_version_id = iter_seg->second;
            }
        }
        if (getTarget(matched_version_id) && !TrafficAllocationTypeEnum::hasTag(reset_traffic_type)) {
            LogUtils::LogInfo(StringPrint("%s: 仅号段分流，命中号段", "abValues"));
            return generateMatchedResultVo(tag, shunt, matched_version_id, conversion_layer_dto, layer_vo);  //生成匹配结果信息
        }
    }

    //2.2 分流字段：圈定字段查找
    const std::unordered_map<std::string, std::unordered_map<std::string, std::vector<int>>> &traffic_tag_mappings = conversion_layer_dto->traffic_tag_mappings;
    //到此处两种情况：圈定字段
    // 1：seg + tag(tag可为空)
    // 1.1 seg已经找到 ，matchedVersionId已经初始化，tag为空则找到返回，不为空则直接去tagmap找版本，找到则返回，找不到走launch
    // 1.2 seg未找到 ，matchedVersionId未初始化，直接走launch
    // 2：纯tag，matchedVersionId未初始化, 已经前置校验不为空，直接去tagmap找版本，找不到走launch
    if (TrafficAllocationTypeEnum::hasTag(reset_traffic_type)) {
        auto iter_tag_map = traffic_tag_mappings.find(traffic_position_id);
        if (TrafficAllocationTypeEnum::hasSeg(reset_traffic_type) && getTarget(matched_version_id)){
            if (VepStringUtils::IsBlank(tag)){
                LogUtils::LogInfo(StringPrint("%s: 号段+圈定字段分流，seg已经找到，圈定字段为空，命中号段", "abValues"));
                return generateMatchedResultVo(tag, shunt, matched_version_id, conversion_layer_dto, layer_vo);
            } else {
                if(iter_tag_map != traffic_tag_mappings.end()) {  //包含，就继续
                    const std::unordered_map<std::string, std::vector<int>> &tag_part = iter_tag_map->second;
                    auto iter_tag = tag_part.find(tag);  //查找tag
                    if (iter_tag != tag_part.end()) { // 找到tag对应的ids
                        const std::vector<int> &id_vec = iter_tag->second;
                        auto it = std::find(id_vec.begin(), id_vec.end(), matched_version_id);
                        if (it != iter_tag->second.end()){  //找到版本
                            LogUtils::LogInfo(StringPrint("%s: 号段+圈定字段分流，seg已经找到，命中号段+圈定字段 ", "abValues"));
                            return generateMatchedResultVo(tag, shunt, matched_version_id, conversion_layer_dto, layer_vo);
                        }
                    }
                }
            }
        } else if (!TrafficAllocationTypeEnum::hasSeg(reset_traffic_type)) {
            // 找到tag对应的ids
            if(iter_tag_map != traffic_tag_mappings.end()) {  //包含，就继续
                const std::unordered_map<std::string, std::vector<int>> &tag_part = iter_tag_map->second;
                auto iter_tag = tag_part.find(tag);  //查找tag
                if (iter_tag != tag_part.end() && !(iter_tag->second).empty()) {
                    matched_version_id = iter_tag->second[0];
                    LogUtils::LogInfo(StringPrint("%s: 仅圈定字段分流，命中圈定字段 ", "abValues"));
                    return generateMatchedResultVo(tag, shunt, matched_version_id, conversion_layer_dto, layer_vo);
                }
            }
        }
    }

    //3.若没有命中白名单、号段，查询是否命中Launch信息
    std::string cache_launch_key;
    LayerCacheKeyGenerator::generateLaunchLayerCacheKey(layer_param_, layer_code, &cache_launch_key);
    std::shared_ptr<ConversionLaunchLayerDto> conversion_launch_layer_dto;
    LayerCacheUtils::getLaunchLayer(cache_launch_key, conversion_launch_layer_dto); //从缓存中取出

    if (conversion_launch_layer_dto->layer_code != "" && !conversion_launch_layer_dto->launch_layer_list.empty()) { //非空类
        const std::unordered_map<std::string, std::shared_ptr<LaunchLayerDto>> &launch_layer_map = conversion_launch_layer_dto->launch_layer_map;   //取出映射关系{流量位id：launch_layer_dto}
        auto iter_launch = launch_layer_map.find(traffic_position_id);
        if (iter_launch != launch_layer_map.end()) {   //找到
            std::shared_ptr<LabVersionDto> empty = std::make_shared<LabVersionDto>();
            const std::shared_ptr<LaunchLayerDto> &launch_layer_dto = iter_launch->second;
            std::shared_ptr<VepApiLabWithPositionVo> empty2 = std::make_shared<VepApiLabWithPositionVo>();
            LogUtils::LogInfo(StringPrint("%s: 命中launch层", "abValues"));
            VepLabHitInfo labHitInfo;
            generateVepLabParamVo(tag, shunt, empty, launch_layer_dto, empty2, layer_vo, &labHitInfo);
            return ResultVo<VepLabHitInfo>(labHitInfo);
        }
    }

    LogUtils::LogInfo(StringPrint("%s: 没有匹配到任何实验", "abValues"));
    ResultVo<VepLabHitInfo> ret(IntegerToString(ResultConstants::LAYER_MATCH_MISSING.first), ResultConstants::LAYER_MATCH_MISSING.second, false);
    return ret;
}

/**
 * 返回匹配的实验信息
 * @param layer_code
 * @param traffic_position_id
 * @param shunt
 * @return
 */
ResultVo<VepLabHitInfo> VepLayerConfigProvider::abValues(const std::string &layer_code, const std::string &traffic_position_id, const std::string &shunt) {
    return abValues(layer_code, traffic_position_id, shunt, "", TrafficAllocationTypeEnum::SEGMENT);
}


/**
 * 根据匹配信息生成结果数据
 * @param shunt
 * @param matched_version_id
 * @param conversion_layer_dto
 * @param layer_vo
 * @return
 */
ResultVo<VepLabHitInfo> VepLayerConfigProvider::generateMatchedResultVo(const std::string &tag, const std::string &shunt, int matched_version_id
        , std::shared_ptr<ConversionLayerDto> conversion_layer_dto
        , const VepApiLayerVo& layer_vo) {
    std::shared_ptr<LabVersionDto> version_dto = conversion_layer_dto->version_map[matched_version_id];   //根据matched_version_id，找到对应的实验版本Vo
    std::shared_ptr<VepApiLabWithPositionVo> lab_with_position_vo = conversion_layer_dto->lab_map[version_dto->lab_id]; //找到实验层流量Vo

    std::shared_ptr<LaunchLayerDto> empty = std::make_shared<LaunchLayerDto>(); //空类
    VepLabHitInfo lab_hitted_info;
    generateVepLabParamVo(tag, shunt, version_dto, empty, lab_with_position_vo, layer_vo, &lab_hitted_info);
    return ResultVo<VepLabHitInfo>(lab_hitted_info);
}

/**
 * 根据匹配信息生成结果类
 * @param shunt 号段
 * @param version_dto 匹配到的实验版本【二选一】
 * @param launch_layer_dto 匹配到的launch版本【二选一】
 * @param lab_with_position_vo 实验层流量位信息
 * @param layer_vo 实验层信息
 * @return
 */
void VepLayerConfigProvider::generateVepLabParamVo(const std::string &tag, const std::string &shunt, std::shared_ptr<LabVersionDto> version_dto
        , std::shared_ptr<LaunchLayerDto> launch_layer_dto
        , std::shared_ptr<VepApiLabWithPositionVo> lab_with_position_vo
        , const VepApiLayerVo& layer_vo
        , VepLabHitInfo* labHitInfo) {
    labHitInfo->shunt = shunt;  //号段
    labHitInfo->tag = tag;

    std::string hash_shunt = calculateHashShunt(shunt, layer_vo);  //计算哈希号段
    labHitInfo->hash_shunt = hash_shunt;

    labHitInfo->factor = layer_vo.salt;
    if(lab_with_position_vo->version_list.empty()) { //为空
        labHitInfo->scene_id = -1;
        labHitInfo->lab_id = -1;
    } else {
        labHitInfo->scene_id = lab_with_position_vo->scene_id;
        labHitInfo->scene_code = lab_with_position_vo->scene_code;
        labHitInfo->lab_id = lab_with_position_vo->id;
        labHitInfo->lab_name = lab_with_position_vo->lab_name;
    }

    if(version_dto->id > 0) {
        LogUtils::LogInfo(StringPrint("%s: version_dto", "generateVepLabParamVo"));
        labHitInfo->lab_group_id = version_dto->id;
        labHitInfo->lab_group_name = version_dto->version_name;
        labHitInfo->parameter_template_code = version_dto->lab_variable_code;
        labHitInfo->parameter_template_name = version_dto->lab_variable_name;

        labHitInfo->params = version_dto->params_with_type;
    } else if (launch_layer_dto->id > 0) {
        LogUtils::LogInfo(StringPrint("%s: launch_layer_dto", "generateVepLabParamVo"));
        labHitInfo->lab_group_id = -1;
        labHitInfo->lab_group_name = "";
        labHitInfo->launch_layer_id = launch_layer_dto->id;
        labHitInfo->parameter_template_code = launch_layer_dto->lab_variable_code;
        labHitInfo->parameter_template_name = launch_layer_dto->lab_variable_name;

        labHitInfo->params = launch_layer_dto->params_with_type;
    }

}

/**
 * 计算生成哈希号段
 * @param shunt
 * @param layer_vo
 * @return
 */
std::string VepLayerConfigProvider::calculateHashShunt(const std::string &shunt, const VepApiLayerVo& layer_vo) {
    std::string hash_shunt = shunt;
    std::string shunt_with_salt = shunt;

    //计算盐值，salt_switch：是否加盐开关，0：关闭，1：打开；salt_location：加盐的位置，1:前面，2：后面；salt：分层因子，盐值；
    int salt_switch = layer_vo.salt_switch;
    if (salt_switch == 1) {
        // 如果加盐
        std::string salt = layer_vo.salt;
        if(!VepStringUtils::IsStringNotblank(salt)){
            salt = "";
        }

        int salt_location = layer_vo.salt_location;
        if(salt_location == 1){
            shunt_with_salt = salt + shunt;
        }else {
            shunt_with_salt = shunt + salt;
        }
    }

    //判断是否需要计算哈希，1：需要计算哈希，0：不需要计算哈希，直接用shunt字段的尾号匹配
    int hash_switch = layer_vo.hash_switch;
    if (hash_switch == 1) {
        //是否哈希MD5加密，1：是，0：否
        int hash_encrypt = layer_vo.encrypt_switch;
        if (hash_encrypt == 1) {
            hash_shunt = VepHashValGenerator::getHashCodeVal(shunt_with_salt);
        } else {
            hash_shunt = IntegerToString(VepHashValGenerator::hashCode(shunt_with_salt));
        }
    } else {
        hash_shunt = shunt_with_salt;
    }

    // 在少数情况下，计算String.hashCode，会出现数值小于3位的情况，需要进行填充
    if ((int)hash_shunt.size() < ConfigConstants::DEFAULT_HASH_SEGMENT_LENGTH) {
        if (StringStartsWith("hash_shunt", "-")) {
            StringPiece strP(hash_shunt);                          //用StringPiece的方法进行截取
            hash_shunt = strP.substr(1).as_string();   //取负号之后是内容
        }
        hash_shunt = VepStringUtils::leftPad(hash_shunt, ConfigConstants::DEFAULT_HASH_SEGMENT_LENGTH, "0");
    }

    return hash_shunt;
}

/**
 * 判断id是否为空
 * @param id
 * @return
 */
bool VepLayerConfigProvider::getTarget(int id) {
    return id > 0;
}

/**
 * 判断流量分配类型是否匹配
 * @param layer_type
 * @param using_type
 * @return
 */
bool VepLayerConfigProvider::validateUsingType(const std::string &layer_type, const std::string &using_type) {
    if (TrafficAllocationTypeEnum::REASONING == using_type) {
        return true;
    }
    return using_type == layer_type;
}

/**
 * 获取分流方式
 * @param shunt
 * @param tag
 * @param using_type
 * @return
 */
std::string VepLayerConfigProvider::resetFromUsingType(const std::string &shunt, const std::string &tag, const std::string &using_type) {
    if (TrafficAllocationTypeEnum::REASONING == using_type) {
        if (VepStringUtils::IsStringNotblank(tag) && VepStringUtils::IsBlank(shunt)) {
            return TrafficAllocationTypeEnum::TAG;
        } else if (VepStringUtils::IsStringNotblank(tag) && VepStringUtils::IsStringNotblank(shunt)){
            return TrafficAllocationTypeEnum::TAG_WITH_SEGMENT;
        } else {
            return TrafficAllocationTypeEnum::SEGMENT;
        }
    }
    return using_type;
}

/**
 * 关闭provider
 */
void VepLayerConfigProvider::close() {
    //LogUtils::LogInfo("VepLayerConfigProvider::close");
    if (watch_scheduled_executor_ != nullptr) {
        //LogUtils::LogInfo("watch_scheduled_executor shutdown...");
        if (!watch_scheduled_executor_->IsTerminated()) {
            watch_scheduled_executor_->Shutdown();
            //watch_scheduled_executor_ = nullptr;
        }
        //LogUtils::LogInfo("watch_scheduled_executor shutdown success");
    }
    if (scheduled_executor_ != nullptr) {
        //LogUtils::LogInfo("scheduled_executor shutdown...");
        if (!scheduled_executor_->IsTerminated()) {
            scheduled_executor_->Shutdown();
            //scheduled_executor_ = nullptr;
        }
        //LogUtils::LogInfo("scheduled_executor shutdown success");
    }
}
