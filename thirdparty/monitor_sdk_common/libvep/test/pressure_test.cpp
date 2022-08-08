//
// Created by 吴婷 on 2020-01-06.
//
#include <iostream>
#include <memory>

#include "thirdparty/monitor_sdk_common/libvep/vep_thirdparty_related_client.h"
#include "thirdparty/monitor_sdk_common/libvep/vep_scene_related_client.h"
#include "thirdparty/monitor_sdk_common/libvep/vep_client.h"

using namespace common;

//std::shared_ptr<VepThirdPartyRelatedClient> vep_related_client_ptr;  //第三方流量位+实验层
//std::shared_ptr<VepSceneRelatedClient> vep_scene_client_ptr;            //场景+实验层
std::shared_ptr<VepClient> vep_client_ptr;                              //场景V1.0

/**
 * 性能压测
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char** argv){

    /**
     * 1.初始化并启动"场景V1"客户端压测
     */
    std::string scene_code = "zzz";
    std::string lab_name = "zLab1220seg_tagA_S";
    vep_client_ptr = std::make_shared<VepClient>(scene_code, "");
    //启动客户端
    long start_init = VepTimeUtils::GetCurrentTimeMicros();
    vep_client_ptr->initialize();
    long end_init = VepTimeUtils::GetCurrentTimeMicros();

    //循环次数
    int loop = 1000 * 10000;
    //开始时间
    long start = VepTimeUtils::GetCurrentTimeMicros();
    for(int i = 0; i < loop; ++i){
        std::string shunt = IntegerToString(868859044744274 + i);
        ResultVo<VepLabParamVo> result_vo = vep_client_ptr->abValueWithSegAndDiscardTag(lab_name, shunt);
        //std::cout << "data: " << result_vo.data.ToString() << std::endl;
    }
    //结束时间
    long end = VepTimeUtils::GetCurrentTimeMicros();
    std::cout << "客户端VepClient初始化, 用时：" << (end_init-start_init) << "微秒\t" << (end_init-start_init)/1000 << " 毫秒" << std::endl;
    std::cout << "客户端VepClient：" << loop << "次查询'abValueWithSegAndTag', 用时：" << (end-start) << "微秒\t" << (end-start)/1000 << " 毫秒" << std::endl;
    vep_client_ptr->close();

    /**
     * 2.初始化并启动"场景+实验层"客户端压测
     */
    /*
    std::string scene_code = "zzz";
    std::string layer_code = "zLayer1220Seg_Tag";
    vep_scene_client_ptr = std::make_shared<VepSceneRelatedClient>(scene_code, layer_code);
    //启动客户端
    long start_init = VepTimeUtils::GetCurrentTimeMicros();
    vep_scene_client_ptr->initialize();
    long end_init = VepTimeUtils::GetCurrentTimeMicros();

    //循环次数
    int loop = 1000 * 10000;
    //开始时间
    long start = VepTimeUtils::GetCurrentTimeMicros();
    for(int i = 0; i < loop; ++i){
        std::string shunt = IntegerToString(868859044744274 + i);
        ResultVo<VepLabHitInfo> result_vo = vep_scene_client_ptr->abValueWithSegAndDiscardTag(layer_code, shunt);
        //std::cout << "data: " << result_vo.data.ToString() << std::endl;
    }
    //结束时间
    long end = VepTimeUtils::GetCurrentTimeMicros();
    std::cout << "客户端VepSceneRelatedClient初始化, 用时：" << (end_init-start_init) << "微秒\t" << (end_init-start_init)/1000 << " 毫秒" << std::endl;
    std::cout << "客户端VepSceneRelatedClient：" << loop << "次查询'abValueWithSegAndTag', 用时：" << (end-start) << "微秒\t" << (end-start)/1000 << " 毫秒" << std::endl;
    vep_scene_client_ptr->close();
    */

    /**
     * 3.初始化并启动"第三方流量位+实验层"客户端压测
     */
    /*
    std::string position_type = TrafficPositionTypeEnum::AD;
    std::string layer_code = "zLayer1220Seg_Tag";
    vep_related_client_ptr = std::make_shared<VepThirdPartyRelatedClient>(position_type, layer_code);
    //启动客户端
    long start_init = VepTimeUtils::GetCurrentTimeMicros();
    vep_related_client_ptr->initialize();
    long end_init = VepTimeUtils::GetCurrentTimeMicros();

    //循环次数
    int loop = 1000 * 10000;
    //开始时间
    long start = VepTimeUtils::GetCurrentTimeMicros();
    for(int i = 0; i < loop; ++i){
        std::string shunt = IntegerToString(868859044744274 + i);
        ResultVo<VepLabHitInfo> result_vo = vep_related_client_ptr->abValueWithSegAndDiscardTag(layer_code, "2", shunt);
        //std::cout << "data: " << result_vo.data.ToString() << std::endl;
    }
    //结束时间
    long end = VepTimeUtils::GetCurrentTimeMicros();
    std::cout << "客户端VepThirdPartyRelatedClient初始化, 用时：" << (end_init-start_init) << "微秒\t" << (end_init-start_init)/1000 << " 毫秒" << std::endl;
    std::cout << "客户端VepThirdPartyRelatedClient：" << loop << "次查询'abValueWithSegAndTag', 用时：" << (end-start) << "微秒\t" << (end-start)/1000 << " 毫秒" << std::endl;
    vep_related_client_ptr->close();
    */
}

