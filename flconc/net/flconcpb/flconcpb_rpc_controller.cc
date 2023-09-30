#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>

#include "flconcpb_rpc_controller.h"

namespace flconc {

    void FlconcPbRpcController::Reset() {}

    bool FlconcPbRpcController::Failed() const {
        return m_is_failed;
    }

    std::string FlconcPbRpcController::ErrorText() const {
        return m_error_info;
    }

    void FlconcPbRpcController::StartCancel() {}

    void FlconcPbRpcController::SetFailed(const std::string &reason) {
        m_is_failed = true;
        m_error_info = reason;
    }

    bool FlconcPbRpcController::IsCanceled() const {
        return false;
    }

    void FlconcPbRpcController::NotifyOnCancel(google::protobuf::Closure *callback) {

    }

    void FlconcPbRpcController::SetErrorCode(const int error_code) {
        m_error_code = error_code;
    }

    int FlconcPbRpcController::ErrorCode() const {
        return m_error_code;
    }

    const std::string &FlconcPbRpcController::MsgSeq() const {
        return m_msg_req;
    }

    void FlconcPbRpcController::SetMsgReq(const std::string &msg_req) {
        m_msg_req = msg_req;
    }

    void FlconcPbRpcController::SetError(const int err_code, const std::string &err_info) {
        SetFailed(err_info);
        SetErrorCode(err_code);
    }

    void FlconcPbRpcController::SetPeerAddr(NetAddress::ptr addr) {
        m_peer_addr = addr;
    }

    void FlconcPbRpcController::SetLocalAddr(NetAddress::ptr addr) {
        m_local_addr = addr;
    }

    NetAddress::ptr FlconcPbRpcController::PeerAddr() {
        return m_peer_addr;
    }

    NetAddress::ptr FlconcPbRpcController::LocalAddr() {
        return m_local_addr;
    }

    void FlconcPbRpcController::SetTimeout(const int timeout) {
        m_timeout = timeout;
    }

    int FlconcPbRpcController::Timeout() const {
        return m_timeout;
    }

    void FlconcPbRpcController::SetMethodName(const std::string &name) {
        m_method_name = name;
    }

    std::string FlconcPbRpcController::GetMethodName() {
        return m_method_name;
    }

    void FlconcPbRpcController::SetMethodFullName(const std::string &name) {
        m_full_name = name;
    }

    std::string FlconcPbRpcController::GetMethodFullName() {
        return m_full_name;
    }

}