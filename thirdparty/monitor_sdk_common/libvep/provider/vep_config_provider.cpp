//
// Created by 吴婷 on 2019-12-10.
//
#include <map>
#include <vector>
#include <exception>
#include <algorithm>
#include <iostream>

#include "thirdparty/monitor_sdk_common/libvep/config/constants.h"
#include "thirdparty/monitor_sdk_common/libvep/config/dynamic_client_config.h"
#include "thirdparty/monitor_sdk_common/libvep/request/vep_req_param.h"
#include "thirdparty/monitor_sdk_common/libvep/util/converter_utils.h"
#include "thirdparty/monitor_sdk_common/libvep/util/deserialize_utils.h"
#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"
#include "thirdparty/monitor_sdk_common/libvep/util/log_utils.h"
#include "vep_config_provider.h"

using namespace common;

int VepConfigProvider::NOT_EXIST_ = -1;

VepConfigProvider::VepConfigProvider(std::shared_ptr <VepConfigService> vep_config_service_ptr,
        std::shared_ptr <ClientReportService> client_report_service_ptr, const VepSceneParamVo &scene_Param_vo) :
        vep_config_service_ptr_(vep_config_service_ptr),
        client_report_service_ptr_(client_report_service_ptr),scene_Param_vo_(scene_Param_vo) {
    initialized_ = false;
    last_accumulate_updated_time_ = -1l;
    last_watched_changing_time_ = -1l;
    last_fetched_changing_time_ = -1l;
}

VepConfigProvider::~VepConfigProvider() {
    close();
}

void VepConfigProvider::initialize() {
    if(!initialized_){
        //初始化调度类
        watch_scheduled_executor_ = std::make_shared<VepScheduledExecutor>(1); //长轮询监听
        report_scheduled_executor_ = std::make_shared<VepScheduledExecutor>(3); //定时拉取上报

        //启动service
        vep_config_service_ptr_->initialize();

        //1.尝试获取远端最新配置
        bool flag = fetchRemoteConfig(FetchRemoteConfigTypeEnum::INITIALIZE, client_report_service_ptr_->generateReportInfo(last_accumulate_updated_time_));

        //2.判断是否启动获取配置成功
        std::shared_ptr<VepApiSceneVo> cache_scene_vo;

        if (flag || LayerCacheUtils::get(scene_Param_vo_.scene_code, cache_scene_vo)) {  //远程拉取成功，或者本地有缓存
            LogUtils::LogInfo(StringPrint("[%s] 尝试启动定时任务", "initialize"));
            initialized_ = true;
            //3.初始化定时任务
            initializeScheduled();
        }  else {
            LogUtils::LogError("VepConfigProvider not initialized. Please Check!");
        }
    }
}

/**
 * gtest测试时，拉取一次
 */
void VepConfigProvider::initializeForGtest() {
    fetchRemoteConfig(FetchRemoteConfigTypeEnum::INITIALIZE, client_report_service_ptr_->generateReportInfo(last_accumulate_updated_time_));
}

/**
 * 获取场景值对象
 * @return
 */
std::shared_ptr<VepApiSceneVo> VepConfigProvider::getSceneVo() {
    //取出场景
    std::shared_ptr<VepApiSceneVo> scene_vo = std::make_shared<VepApiSceneVo>();
    LayerCacheUtils::get(scene_Param_vo_.scene_code, scene_vo);  //从缓存中拉取
    if (scene_vo->last_accumulate_updated_time == -1l) {  //缓存数据为空
        return scene_vo;
    }
    scene_vo = vep_config_service_ptr_->getSceneVO(client_report_service_ptr_->generateReportInfo(last_accumulate_updated_time_));    //远程获取
    return scene_vo;
}

/**
 * 初始化定时任务
 */
void VepConfigProvider::initializeScheduled() {
    //1.开始定时拉取
    report_scheduled_executor_->ScheduleWithFixedDelay([this] (const bool *terminated) {
        LogUtils::LogInfo(StringPrint("[%s] Scheduled fetching config started >> %d", scene_Param_vo_.scene_code, DynamicClientConfig::getScheduledFetchInterval()));
        if((*terminated)) return;
        //获取远端最新配置
        fetchRemoteConfig(FetchRemoteConfigTypeEnum::SCHEDULE, client_report_service_ptr_->generateReportInfo(last_accumulate_updated_time_));
    }, 3000, DynamicClientConfig::getScheduledFetchInterval());

    //2.开始定时监听
    watch_scheduled_executor_->ScheduleWithFixedDelay([this] (const bool *terminated) {
        LogUtils::LogInfo(StringPrint("[%s] Scheduled watching config started >> %d", scene_Param_vo_.scene_code, DynamicClientConfig::getScheduledWatchInterval()));
        if((*terminated)) return;
        watchRemoteConfig();
    }, 100, DynamicClientConfig::getScheduledWatchInterval());

    //3.开始定时上报
    report_scheduled_executor_->ScheduleWithFixedDelay([this] (const bool *terminated) {
        LogUtils::LogInfo(StringPrint("[%s] Scheduled reporting config started >> %d", scene_Param_vo_.scene_code, DynamicClientConfig::getScheduledReportInterval()));
        if((*terminated)) return;
        reportClientInfo();
    }, 3000, DynamicClientConfig::getScheduledReportInterval());

    //配置中心定时拉取
    report_scheduled_executor_->ScheduleWithFixedDelay([this] (const bool *terminated) {
        LogUtils::LogInfo(StringPrint("[%s] 配置中心定时拉取 >> %d", scene_Param_vo_.scene_code, ConfigConstants::SCHEDULED_FETCH_CONFIG_INTERVAL_MILLISECONDS));
        if((*terminated)) return;
        DynamicClientConfig::Instance()->DynamicClientConfigScheduled();
    }, ConfigConstants::SCHEDULED_FETCH_CONFIG_INTERVAL_MILLISECONDS, ConfigConstants::SCHEDULED_FETCH_CONFIG_INTERVAL_MILLISECONDS);
}

/**
 * 监听变更
 * @return
 */
bool VepConfigProvider::watchRemoteConfig() {
    VepSceneReqParam req_param(scene_Param_vo_.scene_code, last_accumulate_updated_time_);  //场景请求参数
    bool has_changed = vep_config_service_ptr_->hasChanged(req_param,
            client_report_service_ptr_->generateReportInfo(last_accumulate_updated_time_, last_watched_changing_time_, last_fetched_changing_time_));
    if(has_changed){
        LogUtils::LogInfo(StringPrint("[%s] scheduled watch changed", scene_Param_vo_.scene_code));
        last_watched_changing_time_ = VepTimeUtils::GetCurrentTimeMills();
        bool flag = fetchRemoteConfig(FetchRemoteConfigTypeEnum::LONG_POLLING,
                                      client_report_service_ptr_->generateReportInfo(last_accumulate_updated_time_));
        if(flag){
            reportClientInfo();
        }
    }
    return has_changed;
}

/**
 * 上报信息
 * @return
 */
bool VepConfigProvider::reportClientInfo() {
    ClientReportParam report_param = client_report_service_ptr_->generateReportInfo(last_accumulate_updated_time_, last_watched_changing_time_, last_fetched_changing_time_);
    bool success_flag = vep_config_service_ptr_->reportClientInfo(report_param);
    LogUtils::LogInfo(StringPrint("[%s] reportClientInfo", scene_Param_vo_.scene_code));
    return success_flag;
}

/**
 * 尝试获取远端最新配置
 * @param type_enum  拉取类型
 * @param report_param 客户端上报参数
 * @return 是否拉取成功
 */
bool VepConfigProvider::fetchRemoteConfig(const std::string &type_enum, const ClientReportParam &report_param) {
    try {
        std::shared_ptr<VepApiSceneVo> scene_vo = std::make_shared<VepApiSceneVo>();
        int max_retry_num = DynamicClientConfig::getFetchFailedCountThreshold();
        for (int i = 0; i < max_retry_num; ++i) {
            //重试获取远端配置，如果失败，判断本地缓存是否存在，如果不存在，则启动报错
            LogUtils::LogInfo(StringPrint("[%s] 尝试获取远端配置", "fetchRemoteConfig"));
            scene_vo = vep_config_service_ptr_->getSceneVO(report_param);    //远程获取
            if (scene_vo->last_accumulate_updated_time != -1l && scene_vo->lab_map.size() > 0) {
                //1.判断last_accumulate_updated_time大小
                std::shared_ptr<VepApiSceneVo> cache_scene_vo = std::make_shared<VepApiSceneVo>();
                LayerCacheUtils::get(scene_Param_vo_.scene_code, cache_scene_vo);  //从缓存中拉取

                if (cache_scene_vo->last_accumulate_updated_time < scene_vo->last_accumulate_updated_time) {
                    //延迟生成MapTagToVersion
                    SceneConverter::generateMapTagToVersionWithinScene(scene_vo);

                    //更新缓存
                    LayerCacheUtils::put(scene_Param_vo_.scene_code, scene_vo);  //放入
                    //设置拉取变更的时间
                    last_fetched_changing_time_ = VepTimeUtils::GetCurrentTimeMills();
                } else if (cache_scene_vo->last_accumulate_updated_time > scene_vo->last_accumulate_updated_time) {
                    //客户端的累积更新时间竟然大于服务器端的时间（理论上不可能出现这种情况）
                    LogUtils::LogWarn(StringPrint("vep accumulateUpdatedTime compare error, clientTime is %s, but serverTime is %s",
                                                  cache_scene_vo->last_accumulate_updated_time, scene_vo->last_accumulate_updated_time));
                    LayerCacheUtils::put(scene_Param_vo_.scene_code, scene_vo);
                }

                //2.更新最后拉取时间
                this->last_accumulate_updated_time_ = scene_vo->last_accumulate_updated_time;
                return true;
            }
        }
        return false;
    } catch (std::exception& e) {
        LogUtils::LogError(StringPrint("VepConfigProvider fetchRemoteConfig error, sceneCode:%s, type_enum:%s", scene_Param_vo_.scene_code, type_enum));
    }
    return false;
}

/**
 * 查询返回值
 * @param lab_name 实验名
 * @param shunt 号段
 * @return
 */
ResultVo<VepLabParamVo> VepConfigProvider::abValues(const std::string &lab_name, const std::string &shunt, const std::string &tag, const std::string &traffic_type) {
    //参数判断
    if (VepStringUtils::IsBlank(lab_name)) { //为空
        ResultVo<VepLabParamVo> ret(IntegerToString(ResultConstants::EMPTY_LAB_NAME.first), ResultConstants::EMPTY_LAB_NAME.second, false);
        return ret;
    }
    if (VepStringUtils::IsBlank(shunt) && VepStringUtils::IsBlank(tag)) { //为空
        ResultVo<VepLabParamVo> ret(IntegerToString(ResultConstants::EMPTY_PARAM.first), ResultConstants::EMPTY_PARAM.second, false);
        return ret;
    }
    //1.取出场景
    std::shared_ptr<VepApiSceneVo> scene_vo = std::make_shared<VepApiSceneVo>();
    LayerCacheUtils::get(scene_Param_vo_.scene_code, scene_vo);  //从缓存中拉取
    if (scene_vo->last_accumulate_updated_time == -1l) {  //缓存数据为空
        LogUtils::LogInfo(StringPrint("结果返回：scene_code: [%s] 缓存为空", scene_Param_vo_.scene_code));
        ResultVo<VepLabParamVo> ret(IntegerToString(ResultConstants::NOT_FOUND_SCENE.first), ResultConstants::NOT_FOUND_SCENE.second, false);
        return ret;
    }
    //2.取出实验值对象
    const std::unordered_map<std::string, std::shared_ptr<VepApiLabVo>> &lab_map = scene_vo->lab_map;
    auto iter_lab = lab_map.find(lab_name);
    if (iter_lab == lab_map.end()) { //没有找到对应的实验
        LogUtils::LogInfo(StringPrint("结果返回：场景[%s]中没有找到相应的实验labName: [%s]!", scene_Param_vo_.scene_code, lab_name));
        ResultVo<VepLabParamVo> ret(IntegerToString(ResultConstants::NOT_FOUND_LAB.first), ResultConstants::NOT_FOUND_LAB.second, false);
        return ret;
    }
    const std::shared_ptr<VepApiLabVo> &lab_vo = iter_lab->second;
    //3.取出实验Vo
    const LabDto &lab = lab_vo->lab;
    if (LabStateEnum::FINISHED.first == lab.lab_state) {
        LogUtils::LogInfo(StringPrint("结果返回：实验[%s]已经完成 ", lab_name));
        ResultVo<VepLabParamVo> ret(IntegerToString(ResultConstants::LAB_HAS_FINISHED.first), ResultConstants::LAB_HAS_FINISHED.second, false);
        return ret;
    }

    //验证使用API与配置是否一致
    const std::string &traffic_allocation_type = VepParseTrafficRuleUtils::grabTrafficRule(lab);
    if (!validateUsingType(traffic_allocation_type, traffic_type)) {
        LogUtils::LogInfo(StringPrint("结果返回：实验[%s]使用api与实验配置方式不匹配!", lab_name));
        ResultVo<VepLabParamVo> ret(IntegerToString(ResultConstants::USING_API_NOT_CONSIST_WITH_LAB_CONFIG.first), ResultConstants::USING_API_NOT_CONSIST_WITH_LAB_CONFIG.second, false);
        return ret;
    }
    std::string reset_traffic_type = resetFromUsingType(shunt, tag, traffic_type);   //分流方式
    if (VepStringUtils::IsBlank(shunt) && TrafficAllocationTypeEnum::hasSeg(reset_traffic_type)) {
        LogUtils::LogInfo(StringPrint("结果返回：实验[%s]号段分流，但号段为空，失败", lab_name));
        ResultVo<VepLabParamVo> ret(IntegerToString(ResultConstants::EMPTY_SHUNT.first), ResultConstants::EMPTY_SHUNT.second, false);
        return ret;
    }
    if (VepStringUtils::IsBlank(tag) && TrafficAllocationTypeEnum::hasTag(reset_traffic_type)
       && !TrafficAllocationTypeEnum::hasSeg(reset_traffic_type)) {
        LogUtils::LogInfo(StringPrint("结果返回：实验[%s]仅圈定字段分流，但圈定字段为空，失败", lab_name));
        ResultVo<VepLabParamVo> ret(IntegerToString(ResultConstants::EMPTY_TAG.first), ResultConstants::EMPTY_TAG.second, false);
        return ret;
    }

    //4.生成返回类
    VepLabParamVo lab_param_vo;
    generateVepLabParamVo(shunt, tag, scene_vo->scene, lab_vo, reset_traffic_type, &lab_param_vo);
    if (!VepStringUtils::IsStringNotblank(lab_param_vo.lab_version_name)) { //为空，没有找到实验版本
        LogUtils::LogInfo(StringPrint("结果返回：实验[%s]没有找到相应的实验版本!", lab_name));
        ResultVo<VepLabParamVo> ret(IntegerToString(ResultConstants::NOT_FOUND_LAB_VERSION.first), ResultConstants::NOT_FOUND_LAB_VERSION.second, false);
        return ret;
    }
    return ResultVo<VepLabParamVo>(lab_param_vo);
}

/**
 * 生成结果类
 * @param shunt
 * @param sceneDto
 * @param lab_vo
 * @return
 */
void VepConfigProvider::generateVepLabParamVo(const std::string &shunt, const std::string &tag, const SceneDto &sceneDto,
                                                    std::shared_ptr<VepApiLabVo> lab_vo,
                                                    const std::string &traffic_allocation_type, VepLabParamVo *lab_param_vo) {
    const LabDto &lab_dto = lab_vo->lab;

    //填充结果类
    lab_param_vo->shunt = shunt;
    lab_param_vo->shunt_tag = tag;
    lab_param_vo->module_id = sceneDto.module_id;
    lab_param_vo->module_code = sceneDto.module_code;
    lab_param_vo->scene_id = sceneDto.id;
    lab_param_vo->scene_code = sceneDto.code;
    lab_param_vo->lab_id = lab_dto.id;
    lab_param_vo->lab_name = lab_dto.lab_name;
    lab_param_vo->factor = lab_dto.factor;
    
    int matched_version_id = -1;

    //1.1有号段时 ：包括 seg, seg+tag两种情况
    if(TrafficAllocationTypeEnum::hasSeg(traffic_allocation_type)){
        std::string hash_shunt = "";
        //计算加密
        hash_shunt = calculateHashShunt(shunt, lab_dto);
        lab_param_vo->hash_shunt = hash_shunt;

        //获取Hash号段尾号长度
        int hash_match_length = lab_vo->hash_match_length;
        if (0 == hash_match_length) {
            hash_match_length = ConfigConstants::DEFAULT_HASH_SEGMENT_LENGTH;
        }
        //计算匹配哪个实验版本
        std::string real_match_val = VepStringUtils::leftPad(hash_shunt, hash_match_length, "0"); //号段的最后3位
        const std::unordered_map<std::string, int> &whitelist_map = lab_vo->whitelist_map;  //白名单
        //白名单 -> 号段
        auto iter_map = whitelist_map.find(shunt);
        if (iter_map != whitelist_map.end()) {  //包含，命中白名单
            matched_version_id = iter_map->second;
            if (getTarget(matched_version_id)) {
                finish(matched_version_id, lab_vo, lab_param_vo);
                LogUtils::LogInfo(StringPrint("实验[%s]: 命中白名单", lab_dto.lab_name));
                return;
            }
        }
        //没有命中白名单，继续匹配查找号段
        const std::unordered_map<std::string, int> &segment_map = lab_vo->segment_map;
        iter_map = segment_map.find(real_match_val);
        if (iter_map != segment_map.end()) {  //包含，命中号段
            matched_version_id = iter_map->second;
            LogUtils::LogInfo(StringPrint("实验[%s]: 命中号段", lab_dto.lab_name));
        }

        //未命中号段（版本）,不管有没有TAG直接快速失败【2个map都没有】
        if (!getTarget(matched_version_id)) {
            LogUtils::LogInfo(StringPrint("结果返回：实验[%s]没有命中任何实验版本", lab_dto.lab_name));
            return;
        }
    }

    //执行到这个地方：两种情况：TAG, TAG+SEG。
    // 后者的SEG已经校验完成并且matchedVersionId初始化。需要二维校验TAG(TAG可能为空)
    // 前者则不用校验SEG，需要校验TAG并初始化matchedVersionId(前置校验TAG不可能为空)。
    if (TrafficAllocationTypeEnum::hasTag(traffic_allocation_type)) { //没命中白名单，继续tag判断
        const std::unordered_map<std::string, std::set<int>> &map_tag_to_version = grabMapTagToVersion(lab_vo); //{tag : vector(versionID)}

        auto iter_vec = map_tag_to_version.find(tag);
        //seg
        if (TrafficAllocationTypeEnum::hasSeg(traffic_allocation_type)) {
            //LogUtils::LogInfo(StringPrint("matched_version_id: %d", matched_version_id));
            if (VepStringUtils::IsStringNotblank(tag)) { //tag不为空
                if (iter_vec == map_tag_to_version.end()) {  //1.直接没有tagMap，失败
                    LogUtils::LogInfo(StringPrint("结果说明：实验[%s]号段+圈定字段分流，tag不为空，没在tagmap中找到对应实验", lab_dto.lab_name));
                    return;
                }
                //2.有tagMap，但map中没有找到对应实验版本
                const std::set<int> &version_set_with_tag = iter_vec->second;
                if (version_set_with_tag.find(matched_version_id) == version_set_with_tag.end()) {
                    LogUtils::LogInfo(StringPrint("结果说明：实验[%s]号段+圈定字段分流，tag不为空，没在tagmap中找到对应实验", lab_dto.lab_name));
                    return;
                }
            }
        } else {
            //纯TAG，没有对应的group项，直接快速失败
            if (iter_vec == map_tag_to_version.end()) {
                LogUtils::LogInfo(StringPrint("结果说明：实验[%s]圈定字段分流，没在tagmap中找到对应实验", lab_dto.lab_name));
                return;
            }
            //集合大小一定为1，如果不是则时出错了
            matched_version_id = *iter_vec->second.begin();
        }
    }
    //实验版本具体内容
    finish(matched_version_id, lab_vo, lab_param_vo);

    LogUtils::LogInfo(StringPrint("结果返回：实验[%s]找到实验内容，成功返回", lab_dto.lab_name));
    return;
}

/**
 * 计算生成哈希号段
 * @param shunt
 * @param lab_dto
 * @return
 */
std::string VepConfigProvider::calculateHashShunt(const std::string &shunt, const LabDto &lab_dto) {
    std::string hash_shunt = shunt;
    std::string shunt_with_salt = shunt;

    //计算盐值，salt_switch：是否加盐开关，0：关闭，1：打开；salt_location：加盐的位置，1:前面，2：后面；salt：分层因子，盐值；
    int salt_switch = lab_dto.salt_switch;
    if (salt_switch == 1) {
        // 如果加盐
        std::string salt = lab_dto.factor;
        if(!VepStringUtils::IsStringNotblank(salt)){
            salt = "";
        }

        int salt_location = lab_dto.salt_location;
        if(salt_location == 1){
            shunt_with_salt = salt + shunt;
        }else {
            shunt_with_salt = shunt + salt;
        }
    }

    //判断是否需要计算哈希，1：需要计算哈希，0：不需要计算哈希，直接用shunt字段的尾号匹配
    int hash_switch = lab_dto.hash_switch;
    if (hash_switch == 1) {
        //是否哈希MD5加密，1：是，0：否
        int hash_encrypt = lab_dto.hash_encrypt;
        if (hash_encrypt == 1) {
            hash_shunt = VepHashValGenerator::getHashCodeVal(shunt_with_salt);
        } else {
            hash_shunt = IntegerToString(VepHashValGenerator::hashCode(shunt_with_salt));
        }
    } else {
        hash_shunt = shunt_with_salt;
    }

    // 在少数情况下，计算String.hashCode，会出现数值小于3位的情况：1.正数：填充；2.负数：绝对值后填充
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
bool VepConfigProvider::getTarget(int id) {
    return id > 0;
}

/**
 * 生成最终返回结果
 * @param matched_version_id
 * @param lab_vo
 * @param lab_param_vo
 */
inline void VepConfigProvider::finish(int matched_version_id, std::shared_ptr<VepApiLabVo> lab_vo, VepLabParamVo* lab_param_vo) {
    lab_param_vo->lab_version_id = matched_version_id; //实验版本id
    const std::unordered_map<int, std::shared_ptr<LabVersionDto>> &version_map = lab_vo->version_map;
    if (!version_map.empty()) {   //1.map不为空
        auto iter = version_map.find(matched_version_id);
        if (iter != version_map.end()) {  //2.找到对应实验
            const std::shared_ptr<LabVersionDto> &matched_lab_version = iter->second;
            lab_param_vo->lab_version_name = matched_lab_version->version_name;
            lab_param_vo->alg_name = matched_lab_version->lab_variable_name;
            lab_param_vo->alg_code = matched_lab_version->lab_variable_code;
            lab_param_vo->baseline_state = matched_lab_version->baseline_state;
            lab_param_vo->traffic_ratio = matched_lab_version->traffic_ratio;
            lab_param_vo->params = matched_lab_version->params_with_type;
        }
    }
}

/**
 * 从lab_vo中取出映射关系
 * @param vep_lab_vo
 * @return
 */
std::unordered_map<std::string, std::set<int>> VepConfigProvider::grabMapTagToVersion(std::shared_ptr<VepApiLabVo> vep_lab_vo) {
    if (vep_lab_vo->tag_map_to_group.empty()) {
        LogUtils::LogInfo(StringPrint("[%s] lab_vo中取出映射关系为空，临时建立映射", "grabMapTagToVersion"));
        rebuild(vep_lab_vo);
    }
    LogUtils::LogInfo(StringPrint("[%s] lab_vo中取出映射关系成功", "grabMapTagToVersion"));
    return vep_lab_vo->tag_map_to_group;
}

void VepConfigProvider::rebuild(std::shared_ptr<VepApiLabVo> vep_lab_vo) {
    SceneConverter::generateMapTagToVersionWithinLab(vep_lab_vo);
}

/**
 * 判断流量分配类型是否匹配
 * @param layer_type
 * @param using_type
 * @return
 */
bool VepConfigProvider::validateUsingType(const std::string &layer_type, const std::string &using_type) {
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
std::string VepConfigProvider::resetFromUsingType(const std::string &shunt, const std::string &tag, const std::string &using_type) {
    if (TrafficAllocationTypeEnum::REASONING == using_type) {
        if(VepStringUtils::IsStringNotblank(tag) && VepStringUtils::IsBlank(shunt)){
            return TrafficAllocationTypeEnum::TAG;
        } else if (VepStringUtils::IsStringNotblank(tag) && VepStringUtils::IsStringNotblank(shunt)) {
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
void VepConfigProvider::close() {
    //std::cout << "VepConfigProvider::close" << std::endl;
    if (watch_scheduled_executor_ != nullptr) {
        //std::cout << "watch_scheduled_executor shutdown" << std::endl;
        if (!watch_scheduled_executor_->IsTerminated()) {
            watch_scheduled_executor_->Shutdown();
            //watch_scheduled_executor_ = nullptr;
        }
    }
    if (report_scheduled_executor_ != nullptr) {
        //std::cout << "report_scheduled_executor shutdown" << std::endl;
        if (!watch_scheduled_executor_->IsTerminated()) {
            watch_scheduled_executor_->Shutdown();
            //watch_scheduled_executor_ = nullptr;
        }
    }
}
