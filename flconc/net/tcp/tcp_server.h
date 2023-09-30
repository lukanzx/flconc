#pragma once

#include <map>
#include <google/protobuf/service.h>

#include "reactor.h"
#include "fd_event.h"
#include "timer.h"
#include "net_address.h"
#include "tcp_connection.h"
#include "io_thread.h"
#include "tcp_connection_time_wheel.h"
#include "abstract_codec.h"
#include "abstract_dispatcher.h"
#include "http_dispatcher.h"
#include "http_servlet.h"

namespace flconc {

    class TcpAcceptor {

    public:

        typedef std::shared_ptr<TcpAcceptor> ptr;

        TcpAcceptor(NetAddress::ptr net_addr);

        void init();

        int toAccept();

        ~TcpAcceptor();

        NetAddress::ptr getPeerAddr() {
            return m_peer_addr;
        }

        NetAddress::ptr geLocalAddr() {
            return m_local_addr;
        }

    private:
        int m_family{-1};
        int m_fd{-1};

        NetAddress::ptr m_local_addr{nullptr};
        NetAddress::ptr m_peer_addr{nullptr};

    };


    class TcpServer {

    public:

        typedef std::shared_ptr<TcpServer> ptr;

        TcpServer(NetAddress::ptr addr, ProtocalType type = FlconcPb_Protocal);

        ~TcpServer();

        void start();

        void addCoroutine(flconc::Coroutine::ptr cor);

        bool registerService(std::shared_ptr<google::protobuf::Service> service);

        bool registerHttpServlet(const std::string &url_path, HttpServlet::ptr servlet);

        TcpConnection::ptr addClient(IOThread *io_thread, int fd);

        void freshTcpConnection(TcpTimeWheel::TcpConnectionSlot::ptr slot);


    public:
        AbstractDispatcher::ptr getDispatcher();

        AbstractCodeC::ptr getCodec();

        NetAddress::ptr getPeerAddr();

        NetAddress::ptr getLocalAddr();

        IOThreadPool::ptr getIOThreadPool();

        TcpTimeWheel::ptr getTimeWheel();


    private:
        void MainAcceptCorFunc();

        void ClearClientTimerFunc();

    private:

        NetAddress::ptr m_addr;

        TcpAcceptor::ptr m_acceptor;

        int m_tcp_counts{0};

        Reactor *m_main_reactor{nullptr};

        bool m_is_stop_accept{false};

        Coroutine::ptr m_accept_cor;

        AbstractDispatcher::ptr m_dispatcher;

        AbstractCodeC::ptr m_codec;

        IOThreadPool::ptr m_io_pool;

        ProtocalType m_protocal_type{FlconcPb_Protocal};

        TcpTimeWheel::ptr m_time_wheel;

        std::map<int, std::shared_ptr<TcpConnection>> m_clients;

        TimerEvent::ptr m_clear_clent_timer_event{nullptr};

    };

}
