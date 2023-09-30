#pragma once

#include <map>
#include <string>

#include "abstract_data.h"
#include "abstract_codec.h"
#include "http_request.h"

namespace flconc {

    class HttpCodeC : public AbstractCodeC {
    public:
        HttpCodeC();

        ~HttpCodeC();

        void encode(TcpBuffer *buf, AbstractData *data);

        void decode(TcpBuffer *buf, AbstractData *data);

        ProtocalType getProtocalType();

    private:
        bool parseHttpRequestLine(HttpRequest *requset, const std::string &tmp);

        bool parseHttpRequestHeader(HttpRequest *requset, const std::string &tmp);

        bool parseHttpRequestContent(HttpRequest *requset, const std::string &tmp);
    };

} 
