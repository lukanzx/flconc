#pragma once

#include <google/protobuf/service.h>
#include <memory>
#include <stdio.h>
#include <functional>

#include "log.h"
#include "tcp_server.h"
#include "timer.h"

namespace flconc {

#define REGISTER_HTTP_SERVLET(path, servlet) \
 do { \
  if(!flconc::GetServer()->registerHttpServlet(path, std::make_shared<servlet>())) { \
    printf("Start flconc server error, because register http servelt error, please look up rpc log get more details!\n"); \
    flconc::Exit(0); \
  } \
 } while(0)\

#define REGISTER_SERVICE(service) \
 do { \
  if (!flconc::GetServer()->registerService(std::make_shared<service>())) { \
    printf("Start flconc server error, because register protobuf service error, please look up rpc log get more details!\n"); \
    flconc::Exit(0); \
  } \
 } while(0)\

    void InitConfig(const char *file);

    void StartRpcServer();

    TcpServer::ptr GetServer();

    int GetIOThreadPoolSize();

    Config::ptr GetConfig();

    void AddTimerEvent(TimerEvent::ptr event);

}
