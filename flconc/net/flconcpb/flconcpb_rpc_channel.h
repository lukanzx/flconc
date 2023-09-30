#pragma once

#include <memory>
#include <google/protobuf/service.h>

#include "net_address.h"
#include "tcp_client.h"

namespace flconc {

    class FlconcPbRpcChannel : public google::protobuf::RpcChannel {

    public:
        typedef std::shared_ptr<FlconcPbRpcChannel> ptr;

        FlconcPbRpcChannel(NetAddress::ptr addr);

        ~FlconcPbRpcChannel() = default;

        void CallMethod(
                const google::protobuf::MethodDescriptor *method,
                google::protobuf::RpcController *controller,
                const google::protobuf::Message *request,
                google::protobuf::Message *response,
                google::protobuf::Closure *done
        );

    private:
        NetAddress::ptr m_addr;

    };

}

