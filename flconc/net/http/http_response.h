#pragma once

#include <string>
#include <memory>

#include "abstract_data.h"
#include "http_define.h"

namespace flconc {

    class HttpResponse : public AbstractData {
    public:
        typedef std::shared_ptr<HttpResponse> ptr;

    public:
        std::string m_response_version;
        int m_response_code;
        std::string m_response_info;
        HttpResponseHeader m_response_header;
        std::string m_response_body;
    };

}


