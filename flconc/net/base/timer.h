#pragma once

#include <time.h>
#include <memory>
#include <map>
#include <functional>

#include "mutex.h"
#include "reactor.h"
#include "fd_event.h"
#include "log.h"


namespace flconc {

    int64_t getNowMs();

    class TimerEvent {

    public:

        typedef std::shared_ptr<TimerEvent> ptr;

        TimerEvent(int64_t interval, bool is_repeated, std::function<void()> task)
                : m_interval(interval), m_is_repeated(is_repeated), m_task(task) {
            m_arrive_time = getNowMs() + m_interval;
            DebugLog << "timeevent will occur at " << m_arrive_time;
        }

        void resetTime() {

            m_arrive_time = getNowMs() + m_interval;

            m_is_cancled = false;
        }

        void wake() {
            m_is_cancled = false;
        }

        void cancle() {
            m_is_cancled = true;
        }

        void cancleRepeated() {
            m_is_repeated = false;
        }

    public:
        int64_t m_arrive_time;
        int64_t m_interval;
        bool m_is_repeated{false};
        bool m_is_cancled{false};
        std::function<void()> m_task;

    };

    class FdEvent;

    class Timer : public flconc::FdEvent {

    public:

        typedef std::shared_ptr<Timer> ptr;

        Timer(Reactor *reactor);

        ~Timer();

        void addTimerEvent(TimerEvent::ptr event, bool need_reset = true);

        void delTimerEvent(TimerEvent::ptr event);

        void resetArriveTime();

        void onTimer();

    private:

        std::multimap<int64_t, TimerEvent::ptr> m_pending_events;
        RWMutex m_event_mutex;

    };

}


