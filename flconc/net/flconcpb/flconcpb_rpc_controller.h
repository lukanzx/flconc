#pragma once

#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>
#include <stdio.h>
#include <memory>

#include "net_address.h"

namespace flconc {

    class FlconcPbRpcController : public google::protobuf::RpcController {

    public:
        typedef std::shared_ptr<FlconcPbRpcController> ptr;

        FlconcPbRpcController() = default;

        ~FlconcPbRpcController() = default;

        void Reset() override;

        bool Failed() const override;

        std::string ErrorText() const override;

        void StartCancel() override;

        void SetFailed(const std::string &reason) override;

        bool IsCanceled() const override;

        void NotifyOnCancel(google::protobuf::Closure *callback) override;

        int ErrorCode() const;

        void SetErrorCode(const int error_code);

        const std::string &MsgSeq() const;

        void SetMsgReq(const std::string &msg_req);

        void SetError(const int err_code, const std::string &err_info);

        void SetPeerAddr(NetAddress::ptr addr);

        void SetLocalAddr(NetAddress::ptr addr);

        NetAddress::ptr PeerAddr();

        NetAddress::ptr LocalAddr();

        void SetTimeout(const int timeout);

        int Timeout() const;

        void SetMethodName(const std::string &name);

        std::string GetMethodName();

        void SetMethodFullName(const std::string &name);

        std::string GetMethodFullName();

    private:
        int m_error_code{0};
        std::string m_error_info;
        std::string m_msg_req;
        bool m_is_failed{false};
        bool m_is_cancled{false};
        NetAddress::ptr m_peer_addr;
        NetAddress::ptr m_local_addr;

        int m_timeout{5000};
        std::string m_method_name;
        std::string m_full_name;

    };

}
