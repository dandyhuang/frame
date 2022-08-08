//
// Created by 吴婷 on 2019-11-23.
//

#ifndef COMMON_LIBVEP_LAYER_CONFIG_SERVICE_H
#define COMMON_LIBVEP_LAYER_CONFIG_SERVICE_H

#include <memory>
#include <map>

#include "thirdparty/monitor_sdk_common/libvep/provider/http/http_invoker.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/http/libcurl_wrapper.h"
#include "thirdparty/monitor_sdk_common/libvep/entity/layer_vo.h"
#include "thirdparty/monitor_sdk_common/libvep/request/vep_req_param.h"

namespace common {
/**
 * 拉取实验层信息
 */
class LayerConfigService {
public:
    LayerConfigService();
    ~LayerConfigService();
    void initialize();

    std::shared_ptr<VepApiLayerTrafficVo> getLayerTrafficVo(const VepLayerReqParam &layer_req_param);  //根据实验层的请求信息，返回实验层的流量信息
    std::shared_ptr<VepApiLaunchLayerVo> getLaunchLayerList(const VepLayerReqParam &layer_req_param);  //根据实验层的请求信息，返回launch层的流量信息
    std::shared_ptr<VepApiLayerFetchVo> hasChanged(const VepLayerReqParam &layer_req_param);           //根据post请求返回更新结果

    std::shared_ptr<VepApiLayerTrafficVo> getLayerTrafficVo(const VepLayerSceneReqParam &scene_req_param);    //根据实验层的请求信息，返回实验层的流量信息【场景】
    std::shared_ptr<VepApiLaunchLayerVo> getLaunchLayerList(const VepLayerSceneReqParam &scene_req_param);    //根据实验层的请求信息，返回launch层的流量信息【场景】
    std::shared_ptr<VepApiLayerFetchVo> hasChanged(const VepLayerSceneReqParam &scene_req_param);             //根据post请求返回更新结果【场景】

private:
    std::shared_ptr<VepApiLayerTrafficVo> obtainLayerTrafficVo(const std::string &req_url, const std::string &layer_req_param); //根据请求url和请求参数，返回实验层的数据类
    std::shared_ptr<VepApiLaunchLayerVo> obtainLaunchLayerList(const std::string &req_url, const std::string &layer_req_param); //根据请求url和请求参数，返回launch层的数据类
    std::shared_ptr<VepApiLayerFetchVo> obtainChangedVal(const std::string &req_url, const std::string &req_param);  //根据请求url和请求参数，返回实验层拉取变更信息

    int acquireMaxRetryNum();   //获取配置信息错误_拉取次数阈值
    bool isHttpApiResultValid(const std::string &result);  //判断http返回结果是否有效
    void init_ibcurl_wrapper();     //初始化libcurl_wrapper

private:
    std::shared_ptr<VepHttpInvoker> http_invoker_ptr_;  //http网络连接客户端指针
    std::shared_ptr<LibcurlWrapper> libcurl_wrapper_;   //可用于http连接超时的客户端
};

}  //namespace common
#endif //COMMON_LIBVEP_LAYER_CONFIG_SERVICE_H
