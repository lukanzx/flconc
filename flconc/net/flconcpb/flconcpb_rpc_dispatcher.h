#pragma once

#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <map>
#include <memory>

#include "abstract_dispatcher.h"
#include "flconcpb_data.h"


namespace flconc {

    class FlconcPbRpcDispacther : public AbstractDispatcher {
    public:

        typedef std::shared_ptr<google::protobuf::Service> service_ptr;

        FlconcPbRpcDispacther() = default;

        ~FlconcPbRpcDispacther() = default;

        void dispatch(AbstractData *data, TcpConnection *conn);

        bool parseServiceFullName(const std::string &full_name, std::string &service_name, std::string &method_name);

        void registerService(service_ptr service);

    public:
        
        std::map<std::string, service_ptr> m_service_map;

    };


}

