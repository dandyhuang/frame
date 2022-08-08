#include "thirdparty/monitor_sdk_common/rpc_proxy/proxy_server.h"

#include <iostream>

#include "thirdparty/monitor_sdk_common/rpc_proxy/proxy_impl.h"

ProxyServer g_app;

void ProxyServer::initialize() {
  // initialize application here:
  addServant<ProxyImp>(tars::ServerConfig::Application + "." +
                      tars::ServerConfig::ServerName + ".TestObj");
}

void ProxyServer::destroyApp() {
  // destroy application here:
}

int main(int argc, char* argv[]) {
  try {
    g_app.main(argc, argv);
    g_app.waitForShutdown();
  } catch (std::exception& e) {
    std::cerr << "std::exception:" << e.what() << std::endl;
  } catch (...) {
    std::cerr << "unknown exception." << std::endl;
  }
  return -1;
}
