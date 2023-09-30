#pragma once

#include <string>
#include <memory>
#include <map>

#include "abstract_data.h"
#include "http_define.h"


namespace flconc {

    class HttpRequest : public AbstractData {
    public:
        typedef std::shared_ptr<HttpRequest> ptr;

    public:
        HttpMethod m_request_method;
        std::string m_request_path;
        std::string m_request_query;
        std::string m_request_version;
        HttpRequestHeader m_requeset_header;
        std::string m_request_body;

        std::map<std::string, std::string> m_query_maps;

    };
}



