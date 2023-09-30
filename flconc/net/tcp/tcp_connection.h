#pragma once

#include <memory>
#include <vector>
#include <queue>

#include "log.h"
#include "fd_event.h"
#include "reactor.h"
#include "tcp_buffer.h"
#include "coroutine.h"
#include "http_request.h"
#include "flconcpb_codec.h"
#include "io_thread.h"
#include "tcp_connection_time_wheel.h"
#include "abstract_slot.h"
#include "net_address.h"
#include "mutex.h"

namespace flconc {

    class TcpServer;

    class TcpClient;

    class IOThread;

    enum TcpConnectionState {
        NotConnected = 1,
        Connected = 2,
        HalfClosing = 3,
        Closed = 4,
    };

    class TcpConnection : public std::enable_shared_from_this<TcpConnection> {

    public:
        typedef std::shared_ptr<TcpConnection> ptr;

        TcpConnection(
                flconc::TcpServer *tcp_svr, flconc::IOThread *io_thread, int fd, int buff_size,
                NetAddress::ptr peer_addr
        );

        TcpConnection(
                flconc::TcpClient *tcp_cli, flconc::Reactor *reactor, int fd, int buff_size,
                NetAddress::ptr peer_addr
        );

        void setUpClient();

        void setUpServer();

        ~TcpConnection();

        void initBuffer(int size);

        enum ConnectionType {
            ServerConnection = 1,
            ClientConnection = 2,
        };

    public:

        void shutdownConnection();

        TcpConnectionState getState();

        void setState(const TcpConnectionState &state);

        TcpBuffer *getInBuffer();

        TcpBuffer *getOutBuffer();

        AbstractCodeC::ptr getCodec() const;

        bool getResPackageData(const std::string &msg_req, FlconcPbStruct::pb_ptr &pb_struct);

        void registerToTimeWheel();

        Coroutine::ptr getCoroutine();

    public:
        void MainServerLoopCorFunc();

        void input();

        void execute();

        void output();

        void setOverTimeFlag(bool value);

        bool getOverTimerFlag();

        void initServer();

    private:
        void clearClient();

    private:
        TcpServer *m_tcp_svr{nullptr};
        TcpClient *m_tcp_cli{nullptr};
        IOThread *m_io_thread{nullptr};
        Reactor *m_reactor{nullptr};

        int m_fd{-1};
        TcpConnectionState m_state{TcpConnectionState::Connected};
        ConnectionType m_connection_type{ServerConnection};

        NetAddress::ptr m_peer_addr;


        TcpBuffer::ptr m_read_buffer;
        TcpBuffer::ptr m_write_buffer;

        Coroutine::ptr m_loop_cor;

        AbstractCodeC::ptr m_codec;

        FdEvent::ptr m_fd_event;

        bool m_stop{false};

        bool m_is_over_time{false};

        std::map<std::string, std::shared_ptr<FlconcPbStruct>> m_reply_datas;

        std::weak_ptr<AbstractSlot<TcpConnection>> m_weak_slot;

        RWMutex m_mutex;

    };

}

