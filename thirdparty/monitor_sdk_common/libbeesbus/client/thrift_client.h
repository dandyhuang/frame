/**
 * Created by 杨民善 on 2020/6/18.
 */

#ifndef BEES_THRIFT_CLIENT_H
#define BEES_THRIFT_CLIENT_H

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../thrift/ThriftSourceProtocol.h"
#include "../util/util.h"
#include "../log/glog_helper.h"

#include <memory>

namespace vivo_bees_bus_cpp_report
{
    class ThriftClient
    {
    public:
        ThriftClient(const std::string &host, const int &port, const int &sendTimeout, const int &recvTimeout);

        virtual ~ThriftClient();

        void init(const std::string &host, const int &port, const int &sendTimeout, const int &recvTimeout);

        inline com::vivo::bees::bus::thrift::ThriftSourceProtocolClient* getClient()
        {
            return m_pClient;
        }

        inline std::shared_ptr <apache::thrift::protocol::TTransport> getTransPort()
        {
            return m_transport;
        }

        inline std::string getHost()
        {
            std::ostringstream buf;
            buf << m_port;
            return m_host + ":" + buf.str();
        }

    private:
        /* Thrift protocol needings... */
        std::shared_ptr <apache::thrift::protocol::TTransport> m_socket;
        std::shared_ptr <apache::thrift::protocol::TTransport> m_transport;
        std::shared_ptr <apache::thrift::protocol::TProtocol> m_protocol;
        com::vivo::bees::bus::thrift::ThriftSourceProtocolClient *m_pClient;
        std::string m_host;
        int m_port;
    };
}

#endif
