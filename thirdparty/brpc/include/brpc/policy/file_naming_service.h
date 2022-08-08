// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.


#ifndef  BRPC_POLICY_FILE_NAMING_SERVICE
#define  BRPC_POLICY_FILE_NAMING_SERVICE

#include "brpc/naming_service.h"


namespace brpc {
namespace policy {

class FileNamingService : public NamingService {
friend class ConsulNamingService;
friend class KubernetesNamingService;
private:
    int RunNamingService(const char* service_name,
                         NamingServiceActions* actions) override;

    int GetServers(const char *service_name,
                   std::vector<ServerNode>* servers);

    void Describe(std::ostream& os, const DescribeOptions&) const override;

    NamingService* New() const override;

    void Destroy() override;
};

}  // namespace policy
} // namespace brpc


#endif  //BRPC_POLICY_FILE_NAMING_SERVICE
