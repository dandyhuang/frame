// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "ThriftSourceProtocol.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using namespace  ::com::vivo::bees::bus::thrift;

class ThriftSourceProtocolHandler : virtual public ThriftSourceProtocolIf {
 public:
  ThriftSourceProtocolHandler() {
    // Your initialization goes here
  }

  Status::type append(const ThriftFlumeEvent& event) {
    // Your implementation goes here
    printf("append\n");

    return com::vivo::bees::bus::thrift::Status::OK;
  }

  Status::type appendBatch(const std::vector<ThriftFlumeEvent> & events) {
    // Your implementation goes here
    printf("appendBatch\n");

    return com::vivo::bees::bus::thrift::Status::OK;
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  ::std::shared_ptr<ThriftSourceProtocolHandler> handler(new ThriftSourceProtocolHandler());
  ::std::shared_ptr<TProcessor> processor(new ThriftSourceProtocolProcessor(handler));
  ::std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  ::std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  ::std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

