//
// Created by 吴婷 on 2020-05-15.
//
#include "utils.h"
#include "thirdparty/monitor_sdk_common/base/string/number.h"

using namespace common;

int main(){

    std::cout << "当前时间: " << IntegerToString(SSOTimeUtils::GetCurrentTimeMills()) << std::endl;

    // json解析
    //std::string data = "{\"code\":-1,\"msg\":\"消息解析失败，请检查参数\",\"redirectUrl\":null,\"data\":null}";
    std::string data = "{\n"
                       "    \"code\": 0,\n"
                       "    \"msg\": \"操作成功\",\n"
                       "    \"redirectUrl\": null,\n"
                       "    \"data\": {\n"
                       "        \"cellStyleMap\": {},\n"
                       "        \"userId\": \"11112727\",\n"
                       "        \"userName\": \"刘建雄\",\n"
                       "        \"verifyPassword\": null,\n"
                       "        \"departmentName\": \"互联网基础平台一部\",\n"
                       "        \"leaderUserId\": \"11073364\",\n"
                       "        \"leaderUserName\": \"石建松\",\n"
                       "        \"remark\": null,\n"
                       "        \"status\": 1,\n"
                       "        \"creator\": \"oa同步\",\n"
                       "        \"createTime\": 1578013250000,\n"
                       "        \"updatePerson\": null,\n"
                       "        \"lastUpdateTime\": 1588704758000,\n"
                       "        \"groupCode\": null,\n"
                       "        \"menuCodeList\": null,\n"
                       "        \"isPrivate\": null\n"
                       "    }\n"
                       "}";

    std::shared_ptr<SSOResponse> response_ptr = std::make_shared<SSOResponse>();
    if(SSOParseUtils::ParseSSOResponse(data, response_ptr)){
        std::cout << "code: " << response_ptr->code << std::endl;
        std::cout << "msg: " << response_ptr->msg << std::endl;
        std::cout << "redirectUrl: " << response_ptr->redirect_url << std::endl;
        std::cout << "data: " << response_ptr->data << std::endl;
    } else{
        std::cout << "解析失败" << std::endl;
    }

    // json转换为map：header转换
    std::string str = "{\n"
                      "\t\"appCode\":\"privilege-web\",\n"
                      "\t\"dateTime\":\"1589769617214\",\n"
                      "\t\"location\":\"cookie\",\n"
                      "\t\"type\":\"token\",\n"
                      "\t\"token\":\"vF5r_KvGXkyqMMK4TB6.kAWnO8vqJL9r7GVqcKYKvsrYg9L9uDZI0G6BYfksxar4bsbxHRe8Hbc*\",\n"
                      "\t\"ip\":\"10.13.184.104\",\n"
                      "\t\"url\":\"http://local.privilege.vivo.xyz:8086/privilege/main\"\n"
                      "}";
    std::map<std::string, std::string> map_str = SSOParseUtils::ParseMap(str);
    std::map<std::string, std::string>::iterator iter;
    for (iter = map_str.begin(); iter != map_str.end(); ++iter) {
        std::cout << iter->first << ":" << iter->second << std::endl;
    }


    return 0;
}
