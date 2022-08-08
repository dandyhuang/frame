#include "thrift_client.h"

namespace vivo_bees_bus_cpp_report
{
    using namespace apache::thrift;
    using namespace apache::thrift::protocol;
    using namespace apache::thrift::transport;

    void ThriftClient::init(const std::string &host, const int &port, const int &sendTimeout, const int &recvTimeout)
    {
        if (host.empty() || port <= 0)
        {
            LOG(ERROR) << "init ThriftClient failed! host: " << host << "|"
                       << "port: " << port << "|"
                       << std::endl;
            return;
        }

        __VIVO_TRY__

        m_host = host;
        m_port = port;
        TSocket *sc = new TSocket(host, port);
        sc->setSendTimeout(sendTimeout > 0 ? sendTimeout : DEFAULT_THRIFT_TIMEOUT);
        sc->setRecvTimeout(recvTimeout > 0 ? recvTimeout : DEFAULT_THRIFT_TIMEOUT);
        sc->setKeepAlive(true);
        std::shared_ptr<apache::thrift::protocol::TTransport> tmp(sc);
        m_socket = tmp;
        m_transport = std::make_shared<TFramedTransport>(m_socket);
        m_protocol = std::make_shared<TCompactProtocol>(m_transport);
        m_pClient = new com::vivo::bees::bus::thrift::ThriftSourceProtocolClient(m_protocol);

        LOG(INFO) << "host: " << host << "|" << "port: " << port << std::endl;
        __VIVO_CATCH_WITH_EXCEPTION__
    }

    ThriftClient::ThriftClient(const std::string &host, const int &port, const int &sendTimeout, const int &recvTimeout)
    {
        init(host, port, sendTimeout, recvTimeout);
    }

    ThriftClient::~ThriftClient()
    {
        m_transport->close();

        if (m_pClient)
        {
            delete m_pClient;
            m_pClient = NULL;
        }
    }
}