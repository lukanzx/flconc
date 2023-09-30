#pragma once

#include <string>
#include <memory>

#include "tcp_buffer.h"
#include "abstract_data.h"

namespace flconc {

    enum ProtocalType {
        FlconcPb_Protocal = 1,
        Http_Protocal = 2
    };

    class AbstractCodeC {

    public:
        typedef std::shared_ptr<AbstractCodeC> ptr;

        AbstractCodeC() {}

        virtual ~AbstractCodeC() {}

        virtual void encode(TcpBuffer *buf, AbstractData *data) = 0;

        virtual void decode(TcpBuffer *buf, AbstractData *data) = 0;

        virtual ProtocalType getProtocalType() = 0;

    };

}


