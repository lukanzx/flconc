#include <memory>
#include <google/protobuf/service.h>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#include "net_address.h"
#include "error_code.h"
#include "tcp_client.h"
#include "flconcpb_rpc_channel.h"
#include "flconcpb_rpc_controller.h"
#include "flconcpb_codec.h"
#include "flconcpb_data.h"
#include "log.h"
#include "msg_req.h"
#include "run_time.h"

namespace flconc {

    FlconcPbRpcChannel::FlconcPbRpcChannel(NetAddress::ptr addr) : m_addr(addr) {

    }

    void FlconcPbRpcChannel::CallMethod(
            const google::protobuf::MethodDescriptor *method,
            google::protobuf::RpcController *controller,
            const google::protobuf::Message *request,
            google::protobuf::Message *response,
            google::protobuf::Closure *done
    ) {

        FlconcPbStruct pb_struct;
        FlconcPbRpcController *rpc_controller = dynamic_cast<FlconcPbRpcController *>(controller);
        if (!rpc_controller) {
            ErrorLog << "call failed. falid to dynamic cast FlconcPbRpcController";
            return;
        }

        TcpClient::ptr m_client = std::make_shared<TcpClient>(m_addr);
        rpc_controller->SetLocalAddr(m_client->getLocalAddr());
        rpc_controller->SetPeerAddr(m_client->getPeerAddr());

        pb_struct.service_full_name = method->full_name();
        DebugLog << "call service_name = " << pb_struct.service_full_name;
        if (!request->SerializeToString(&(pb_struct.pb_data))) {
            ErrorLog << "serialize send package error";
            return;
        }

        if (!rpc_controller->MsgSeq().empty()) {
            pb_struct.msg_req = rpc_controller->MsgSeq();
        } else {

            RunTime *run_time = getCurrentRunTime();
            if (run_time != NULL && !run_time->m_msg_no.empty()) {
                pb_struct.msg_req = run_time->m_msg_no;
                DebugLog << "get from RunTime succ, msgno = " << pb_struct.msg_req;
            } else {
                pb_struct.msg_req = MsgReqUtil::genMsgNumber();
                DebugLog << "get from RunTime error, generate new msgno = " << pb_struct.msg_req;
            }
            rpc_controller->SetMsgReq(pb_struct.msg_req);
        }

        AbstractCodeC::ptr m_codec = m_client->getConnection()->getCodec();
        m_codec->encode(m_client->getConnection()->getOutBuffer(), &pb_struct);
        if (!pb_struct.encode_succ) {
            rpc_controller->SetError(ERROR_FAILED_ENCODE, "encode flconcpb data error");
            return;
        }

        InfoLog << "============================================================";
        InfoLog << pb_struct.msg_req << "|" << rpc_controller->PeerAddr()->toString()
                << "|. Set client send request data:" << request->ShortDebugString();
        InfoLog << "============================================================";
        m_client->setTimeout(rpc_controller->Timeout());

        FlconcPbStruct::pb_ptr res_data;
        int rt = m_client->sendAndRecvFlconcPb(pb_struct.msg_req, res_data);
        if (rt != 0) {
            rpc_controller->SetError(rt, m_client->getErrInfo());
            ErrorLog << pb_struct.msg_req << "|call rpc occur client error, service_full_name="
                     << pb_struct.service_full_name << ", error_code="
                     << rt << ", error_info = " << m_client->getErrInfo();
            return;
        }

        if (!response->ParseFromString(res_data->pb_data)) {
            rpc_controller->SetError(ERROR_FAILED_DESERIALIZE, "failed to deserialize data from server");
            ErrorLog << pb_struct.msg_req << "|failed to deserialize data";
            return;
        }
        if (res_data->err_code != 0) {
            ErrorLog << pb_struct.msg_req << "|server reply error_code=" << res_data->err_code << ", err_info="
                     << res_data->err_info;
            rpc_controller->SetError(res_data->err_code, res_data->err_info);
            return;
        }

        InfoLog << "============================================================";
        InfoLog << pb_struct.msg_req << "|" << rpc_controller->PeerAddr()->toString()
                << "|call rpc server [" << pb_struct.service_full_name << "] succ"
                << ". Get server reply response data:" << response->ShortDebugString();
        InfoLog << "============================================================";


        if (done) {
            done->Run();
        }
    }

}