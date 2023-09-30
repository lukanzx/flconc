#pragma once

#include <google/protobuf/service.h>
#include <future>

#include "flconcpb_data.h"
#include "flconcpb_rpc_channel.h"
#include "flconcpb_rpc_controller.h"
#include "net_address.h"
#include "tcp_client.h"
#include "coroutine.h"

namespace flconc {

    class FlconcPbRpcAsyncChannel
            : public google::protobuf::RpcChannel, public std::enable_shared_from_this<FlconcPbRpcAsyncChannel> {

    public:
        typedef std::shared_ptr<FlconcPbRpcAsyncChannel> ptr;
        typedef std::shared_ptr<google::protobuf::RpcController> con_ptr;
        typedef std::shared_ptr<google::protobuf::Message> msg_ptr;
        typedef std::shared_ptr<google::protobuf::Closure> clo_ptr;

        FlconcPbRpcAsyncChannel(NetAddress::ptr addr);

        ~FlconcPbRpcAsyncChannel();

        void CallMethod(
                const google::protobuf::MethodDescriptor *method,
                google::protobuf::RpcController *controller,
                const google::protobuf::Message *request,
                google::protobuf::Message *response,
                google::protobuf::Closure *done
        );

        FlconcPbRpcChannel *getRpcChannel();

        void saveCallee(con_ptr controller, msg_ptr req, msg_ptr res, clo_ptr closure);

        void wait();

        void setFinished(bool value);

        bool getNeedResume();

        IOThread *getIOThread();

        Coroutine *getCurrentCoroutine();

        google::protobuf::RpcController *getControllerPtr();

        google::protobuf::Message *getRequestPtr();

        google::protobuf::Message *getResponsePtr();

        google::protobuf::Closure *getClosurePtr();

    private:
        FlconcPbRpcChannel::ptr m_rpc_channel;
        Coroutine::ptr m_pending_cor;
        Coroutine *m_current_cor{NULL};
        IOThread *m_current_iothread{NULL};
        bool m_is_finished{false};
        bool m_need_resume{false};
        bool m_is_pre_set{false};

    private:
        con_ptr m_controller;
        msg_ptr m_req;
        msg_ptr m_res;
        clo_ptr m_closure;

    };

}

