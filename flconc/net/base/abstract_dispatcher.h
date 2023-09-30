#pragma once

#include <memory>
#include <google/protobuf/service.h>

#include "abstract_data.h"
#include "tcp_connection.h"

namespace flconc {

    class TcpConnection;
    class AbstractDispatcher {
    public:
        typedef std::shared_ptr<AbstractDispatcher> ptr;

        AbstractDispatcher() {}

        virtual ~AbstractDispatcher() {}

        virtual void dispatch(AbstractData *data, TcpConnection *conn) = 0;

    };

}


