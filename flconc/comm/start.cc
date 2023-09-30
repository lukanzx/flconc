#include <google/protobuf/service.h>

#include "start.h"
#include "log.h"
#include "config.h"
#include "tcp_server.h"
#include "coroutine_hook.h"

namespace flconc {

    flconc::Config::ptr gRpcConfig;
    flconc::Logger::ptr gRpcLogger;
    flconc::TcpServer::ptr gRpcServer;

    static int g_init_config = 0;

    void InitConfig(const char *file) {
        flconc::SetHook(false);

#ifdef DECLARE_MYSQL_PULGIN
        int rt = mysql_library_init(0, NULL, NULL);
        if (rt != 0) {
          printf("Start flconc server error, call mysql_library_init error\n");
          mysql_library_end();
          exit(0);
        }
#endif

        flconc::SetHook(true);

        if (g_init_config == 0) {
            gRpcConfig = std::make_shared<flconc::Config>(file);
            gRpcConfig->readConf();
            g_init_config = 1;
        }
    }
    
    TcpServer::ptr GetServer() {
        return gRpcServer;
    }

    void StartRpcServer() {
        gRpcLogger->start();
        gRpcServer->start();
    }

    int GetIOThreadPoolSize() {
        return gRpcServer->getIOThreadPool()->getIOThreadPoolSize();
    }

    Config::ptr GetConfig() {
        return gRpcConfig;
    }

    void AddTimerEvent(TimerEvent::ptr event) {

    }

}