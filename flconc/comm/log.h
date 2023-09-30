#pragma once

#include <sstream>
#include <sstream>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <unistd.h>
#include <memory>
#include <vector>
#include <queue>
#include <semaphore.h>

#include "mutex.h"
#include "config.h"

namespace flconc {

    extern flconc::Config::ptr gRpcConfig;

    template<typename... Args>
    std::string formatString(const char *str, Args &&... args) {

        int size = snprintf(nullptr, 0, str, args...);

        std::string result;
        if (size > 0) {
            result.resize(size);
            snprintf(&result[0], size + 1, str, args...);
        }

        return result;
    }

#define DebugLog \
    if (flconc::OpenLog() && flconc::LogLevel::DEBUG >= flconc::gRpcConfig->m_log_level) \
        flconc::LogTmp(flconc::LogEvent::ptr(new flconc::LogEvent(flconc::LogLevel::DEBUG, __FILE__, __LINE__, __func__, flconc::LogType::RPC_LOG))).getStringStream()

#define InfoLog \
    if (flconc::OpenLog() && flconc::LogLevel::INFO >= flconc::gRpcConfig->m_log_level) \
        flconc::LogTmp(flconc::LogEvent::ptr(new flconc::LogEvent(flconc::LogLevel::INFO, __FILE__, __LINE__, __func__, flconc::LogType::RPC_LOG))).getStringStream()

#define WarnLog \
    if (flconc::OpenLog() && flconc::LogLevel::WARN >= flconc::gRpcConfig->m_log_level) \
        flconc::LogTmp(flconc::LogEvent::ptr(new flconc::LogEvent(flconc::LogLevel::WARN, __FILE__, __LINE__, __func__, flconc::LogType::RPC_LOG))).getStringStream()

#define ErrorLog \
    if (flconc::OpenLog() && flconc::LogLevel::ERROR >= flconc::gRpcConfig->m_log_level) \
        flconc::LogTmp(flconc::LogEvent::ptr(new flconc::LogEvent(flconc::LogLevel::ERROR, __FILE__, __LINE__, __func__, flconc::LogType::RPC_LOG))).getStringStream()


#define AppDebugLog(str, ...) \
  if (flconc::OpenLog() && flconc::LogLevel::DEBUG >= flconc::gRpcConfig->m_app_log_level) \
  { \
    flconc::Logger::GetLogger()->pushAppLog(flconc::LogEvent(flconc::LogLevel::DEBUG, __FILE__, __LINE__, __func__, flconc::LogType::APP_LOG).toString() \
      + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + flconc::formatString(str, ##__VA_ARGS__) + "\n");\
  } \

#define AppInfoLog(str, ...) \
  if (flconc::OpenLog() && flconc::LogLevel::INFO>= flconc::gRpcConfig->m_app_log_level) \
  { \
    flconc::Logger::GetLogger()->pushAppLog(flconc::LogEvent(flconc::LogLevel::INFO, __FILE__, __LINE__, __func__, flconc::LogType::APP_LOG).toString() \
      + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + flconc::formatString(str, ##__VA_ARGS__) + "\n");\
  } \

#define AppWarnLog(str, ...) \
  if (flconc::OpenLog() && flconc::LogLevel::WARN>= flconc::gRpcConfig->m_app_log_level) \
  { \
    flconc::Logger::GetLogger()->pushAppLog(flconc::LogEvent(flconc::LogLevel::WARN, __FILE__, __LINE__, __func__, flconc::LogType::APP_LOG).toString() \
      + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + flconc::formatString(str, ##__VA_ARGS__) + "\n");\
  } \

#define AppErrorLog(str, ...) \
  if (flconc::OpenLog() && flconc::LogLevel::ERROR>= flconc::gRpcConfig->m_app_log_level) \
  { \
    flconc::Logger::GetLogger()->pushAppLog(flconc::LogEvent(flconc::LogLevel::ERROR, __FILE__, __LINE__, __func__, flconc::LogType::APP_LOG).toString() \
      + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + flconc::formatString(str, ##__VA_ARGS__) + "\n");\
  } \


    enum LogType {
        RPC_LOG = 1,
        APP_LOG = 2,
    };

    pid_t gettid();

    LogLevel stringToLevel(const std::string &str);

    std::string levelToString(LogLevel level);

    bool OpenLog();

    class LogEvent {

    public:

        typedef std::shared_ptr<LogEvent> ptr;

        LogEvent(LogLevel level, const char *file_name, int line, const char *func_name, LogType type);

        ~LogEvent();

        std::stringstream &getStringStream();

        std::string toString();

        void log();

    private:

        timeval m_timeval;
        LogLevel m_level;
        pid_t m_pid{0};
        pid_t m_tid{0};
        int m_cor_id{0};

        const char *m_file_name;
        int m_line{0};
        const char *m_func_name;
        LogType m_type;
        std::string m_msg_no;

        std::stringstream m_ss;
    };

    class LogTmp {

    public:
        explicit LogTmp(LogEvent::ptr event);

        ~LogTmp();

        std::stringstream &getStringStream();

    private:
        LogEvent::ptr m_event;

    };

    class AsyncLogger {
    public:
        typedef std::shared_ptr<AsyncLogger> ptr;

        AsyncLogger(const char *file_name, const char *file_path, int max_size, LogType logtype);

        ~AsyncLogger();

        void push(std::vector<std::string> &buffer);

        void flush();

        static void *excute(void *);

        void stop();

    public:
        std::queue<std::vector<std::string>> m_tasks;

    private:
        const char *m_file_name;
        const char *m_file_path;
        int m_max_size{0};
        LogType m_log_type;
        int m_no{0};
        bool m_need_reopen{false};
        FILE *m_file_handle{nullptr};
        std::string m_date;

        Mutex m_mutex;
        pthread_cond_t m_condition;
        bool m_stop{false};

    public:
        pthread_t m_thread;
        sem_t m_semaphore;

    };

    class Logger {

    public:
        static Logger *GetLogger();

    public:
        typedef std::shared_ptr<Logger> ptr;

        Logger();

        ~Logger();

        void init(const char *file_name, const char *file_path, int max_size, int sync_inteval);

        void pushRpcLog(const std::string &log_msg);

        void pushAppLog(const std::string &log_msg);

        void loopFunc();

        void flush();

        void start();

        AsyncLogger::ptr getAsyncLogger() {
            return m_async_rpc_logger;
        }

        AsyncLogger::ptr getAsyncAppLogger() {
            return m_async_app_logger;
        }

    public:
        std::vector<std::string> m_buffer;
        std::vector<std::string> m_app_buffer;

    private:
        Mutex m_app_buff_mutex;
        Mutex m_buff_mutex;
        bool m_is_init{false};
        AsyncLogger::ptr m_async_rpc_logger;
        AsyncLogger::ptr m_async_app_logger;

        int m_sync_inteval{0};

    };

    void Exit(int code);

}

