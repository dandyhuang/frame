// @copyright vivo
// author: yuchengzhen
#pragma once

#include "servant/Application.h"

class ProxyServer : public Application {
 public:
  virtual ~ProxyServer(){};

  virtual void initialize();

  virtual void destroyApp();
};

extern ProxyServer g_app;

