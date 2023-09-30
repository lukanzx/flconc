#pragma once

#include <memory>
#include <google/protobuf/service.h>

#include "coroutine.h"
#include "coroutine_hook.h"
#include "net_address.h"
#include "reactor.h"
#include "tcp_connection.h"
#include "abstract_codec.h"

namespace flconc {

    class TcpClient {
    public:
        typedef std::shared_ptr<TcpClient> ptr;

        TcpClient(NetAddress::ptr addr, ProtocalType type = FlconcPb_Protocal);

        ~TcpClient();

        void init();

        void resetFd();

        int sendAndRecvFlconcPb(const std::string &msg_no, FlconcPbStruct::pb_ptr &res);

        void stop();

        TcpConnection *getConnection();

        void setTimeout(const int v) {
            m_max_timeout = v;
        }

        void setTryCounts(const int v) {
            m_try_counts = v;
        }

        const std::string &getErrInfo() {
            return m_err_info;
        }

        NetAddress::ptr getPeerAddr() const {
            return m_peer_addr;
        }

        NetAddress::ptr getLocalAddr() const {
            return m_local_addr;
        }

        AbstractCodeC::ptr getCodeC() {
            return m_codec;
        }

    private:

        int m_family{0};
        int m_fd{-1};
        int m_try_counts{3};
        int m_max_timeout{10000};
        bool m_is_stop{false};
        std::string m_err_info;

        NetAddress::ptr m_local_addr{nullptr};
        NetAddress::ptr m_peer_addr{nullptr};
        Reactor *m_reactor{nullptr};
        TcpConnection::ptr m_connection{nullptr};

        AbstractCodeC::ptr m_codec{nullptr};

        bool m_connect_succ{false};

    };

}

