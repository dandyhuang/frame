//
// Created by 吴婷 on 2019-11-28.
//

#ifndef COMMON_LIBVEP_VEP_CONFIG_SERVICE_H
#define COMMON_LIBVEP_VEP_CONFIG_SERVICE_H

#include <memory>

#include "thirdparty/monitor_sdk_common/libvep/entity/scene_vo.h"
#include "thirdparty/monitor_sdk_common/libvep/request/client_report_param.h"
#include "thirdparty/monitor_sdk_common/libvep/request/vep_req_param.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/http/http_invoker.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/http/libcurl_wrapper.h"
#include "thirdparty/monitor_sdk_common/libvep/util/log_utils.h"

namespace common {
/**
 * vep配置服务
 */
class VepConfigService {
public:
    VepConfigService();
    ~VepConfigService();
    explicit VepConfigService(const VepSceneParamVo &scene_param_vo);
    void initialize();

    std::shared_ptr<VepApiSceneVo> getSceneVO(const ClientReportParam &client_report_param);                  //获取场景值对象
    bool hasChanged(VepSceneReqParam req_param, const ClientReportParam &client_report_param);//判断接口是否改变
    bool reportClientInfo(const ClientReportParam &client_report_param);     //获取上报信息

    static std::string obtainApiServerUrl();                //默认的接口服务地址
    static std::string obtainClientReportApiServerUrl();    //客户端上报日志_接口服务地址
    static int obtainRequestTimeoutMilliseconds();          //请求超时时间
    static int obtainLongPollingTimeoutMilliseconds();      //请求长轮询_超时时间

private:
    void init_ibcurl_wrapper();     //初始化libcurl_wrapper

private:
    VepSceneParamVo scene_param_vo_;
    std::shared_ptr<VepHttpInvoker> http_invoker_ptr_;  //http连接客户端指针
    std::shared_ptr<LibcurlWrapper> libcurl_wrapper_;   //可用于http连接超时的客户端
};

}//namespace common

#endif //COMMON_LIBVEP_VEP_CONFIG_SERVICE_H
