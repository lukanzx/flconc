#pragma once

#include <queue>
#include <vector>

#include "abstract_slot.h"
#include "reactor.h"
#include "timer.h"

namespace flconc {

    class TcpConnection;

    class TcpTimeWheel {

    public:
        typedef std::shared_ptr<TcpTimeWheel> ptr;

        typedef AbstractSlot<TcpConnection> TcpConnectionSlot;

        TcpTimeWheel(Reactor *reactor, int bucket_count, int invetal = 10);

        ~TcpTimeWheel();

        void fresh(TcpConnectionSlot::ptr slot);

        void loopFunc();

    private:
        Reactor *m_reactor{nullptr};
        int m_bucket_count{0};
        int m_inteval{0};

        TimerEvent::ptr m_event;
        std::queue<std::vector<TcpConnectionSlot::ptr>> m_wheel;
    };

}
