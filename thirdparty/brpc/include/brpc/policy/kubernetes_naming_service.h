// Authors: weitao.zhang@vivo.com

#ifndef  BRPC_POLICY_KUBERNETES_NAMING_SERVICE
#define  BRPC_POLICY_KUBERNETES_NAMING_SERVICE

#include "brpc/naming_service.h"
#include "brpc/channel.h"


namespace brpc {
class Channel;
namespace policy {

class KubernetesNamingService : public NamingService {
private:
    int RunNamingService(const char* service_name,
                         NamingServiceActions* actions) override;

    int GetServers(const char* service_name,
                   std::vector<ServerNode>* servers);

    void Describe(std::ostream& os, const DescribeOptions&) const override;

    NamingService* New() const override;

    int DegradeToOtherServiceIfNeeded(const char* service_name,
                                      std::vector<ServerNode>* servers);

    bool IsServersUpdate(std::vector<ServerNode>* servers);

    void Destroy() override;

private:
    Channel _channel;
    std::string _kubernetes_url;
    bool _backup_file_loaded = false;
    bool _kubernetes_connected = false;
    std::vector<ServerNode> _last_servers;
    // std::vector<std::string> params_;
};

}  // namespace policy
} // namespace brpc


#endif  // BRPC_POLICY_KUBERNETES_NAMING_SERVICE
