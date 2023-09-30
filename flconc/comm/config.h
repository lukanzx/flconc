#pragma once

#include <string>
#include <memory>
#include <map>
#include <tinyxml.h>

#ifdef DECLARE_MYSQL_PLUGIN
#include "mysql_instase.h"
#endif

namespace flconc {

    enum LogLevel {
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        NONE = 5
    };

    class Config {

    public:
        typedef std::shared_ptr<Config> ptr;

        Config(const char *file_path);

        ~Config();

        void readConf();

        void readDBConfig(TiXmlElement *node);

        void readLogConfig(TiXmlElement *node);

        TiXmlElement *getXmlNode(const std::string &name);

    public:

        std::string m_log_path;
        std::string m_log_prefix;
        int m_log_max_size{0};
        LogLevel m_log_level{LogLevel::DEBUG};
        LogLevel m_app_log_level{LogLevel::DEBUG};
        int m_log_sync_inteval{500};

        int m_cor_stack_size{0};
        int m_cor_pool_size{0};

        int m_msg_req_len{0};

        int m_max_connect_timeout{0};
        int m_iothread_num{0};

        int m_timewheel_bucket_num{0};
        int m_timewheel_inteval{0};

#ifdef DECLARE_MYSQL_PLUGIN
        std::map<std::string, MySQLOption> m_mysql_options;
#endif

    private:
        std::string m_file_path;

        TiXmlDocument *m_xml_file;

    };

}

