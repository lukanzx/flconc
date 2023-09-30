#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "coroutine_hook.h"
#include "coroutine.h"
#include "fd_event.h"
#include "reactor.h"
#include "timer.h"
#include "log.h"
#include "config.h"

#define HOOK_SYS_FUNC(name) name##_fun_ptr_t g_sys_##name##_fun = (name##_fun_ptr_t)dlsym(RTLD_NEXT, #name);


HOOK_SYS_FUNC(accept);
HOOK_SYS_FUNC(read);
HOOK_SYS_FUNC(write);
HOOK_SYS_FUNC(connect);
HOOK_SYS_FUNC(sleep);

namespace flconc {

    extern flconc::Config::ptr gRpcConfig;

    static bool g_hook = true;

    void SetHook(bool value) {
        g_hook = value;
    }

    void toEpoll(flconc::FdEvent::ptr fd_event, int events) {

        flconc::Coroutine *cur_cor = flconc::Coroutine::GetCurrentCoroutine();
        if (events & flconc::IOEvent::READ) {
            DebugLog << "fd:[" << fd_event->getFd() << "], register read event to epoll";

            fd_event->setCoroutine(cur_cor);
            fd_event->addListenEvents(flconc::IOEvent::READ);
        }
        if (events & flconc::IOEvent::WRITE) {
            DebugLog << "fd:[" << fd_event->getFd() << "], register write event to epoll";

            fd_event->setCoroutine(cur_cor);
            fd_event->addListenEvents(flconc::IOEvent::WRITE);
        }

    }

    ssize_t read_hook(int fd, void *buf, size_t count) {
        DebugLog << "this is hook read";
        if (flconc::Coroutine::IsMainCoroutine()) {
            DebugLog << "hook disable, call sys read func";
            return g_sys_read_fun(fd, buf, count);
        }

        flconc::Reactor::GetReactor();

        flconc::FdEvent::ptr fd_event = flconc::FdEventContainer::GetFdContainer()->getFdEvent(fd);
        if (fd_event->getReactor() == nullptr) {
            fd_event->setReactor(flconc::Reactor::GetReactor());
        }

        fd_event->setNonBlock();

        ssize_t n = g_sys_read_fun(fd, buf, count);
        if (n > 0) {
            return n;
        }

        toEpoll(fd_event, flconc::IOEvent::READ);

        DebugLog << "read func to yield";
        flconc::Coroutine::Yield();

        fd_event->delListenEvents(flconc::IOEvent::READ);
        fd_event->clearCoroutine();

        DebugLog << "read func yield back, now to call sys read";
        return g_sys_read_fun(fd, buf, count);

    }

    int accept_hook(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
        DebugLog << "this is hook accept";
        if (flconc::Coroutine::IsMainCoroutine()) {
            DebugLog << "hook disable, call sys accept func";
            return g_sys_accept_fun(sockfd, addr, addrlen);
        }
        flconc::Reactor::GetReactor();

        flconc::FdEvent::ptr fd_event = flconc::FdEventContainer::GetFdContainer()->getFdEvent(sockfd);
        if (fd_event->getReactor() == nullptr) {
            fd_event->setReactor(flconc::Reactor::GetReactor());
        }

        fd_event->setNonBlock();

        int n = g_sys_accept_fun(sockfd, addr, addrlen);
        if (n > 0) {
            return n;
        }

        toEpoll(fd_event, flconc::IOEvent::READ);

        DebugLog << "accept func to yield";
        flconc::Coroutine::Yield();

        fd_event->delListenEvents(flconc::IOEvent::READ);
        fd_event->clearCoroutine();


        DebugLog << "accept func yield back, now to call sys accept";
        return g_sys_accept_fun(sockfd, addr, addrlen);

    }

    ssize_t write_hook(int fd, const void *buf, size_t count) {
        DebugLog << "this is hook write";
        if (flconc::Coroutine::IsMainCoroutine()) {
            DebugLog << "hook disable, call sys write func";
            return g_sys_write_fun(fd, buf, count);
        }
        flconc::Reactor::GetReactor();

        flconc::FdEvent::ptr fd_event = flconc::FdEventContainer::GetFdContainer()->getFdEvent(fd);
        if (fd_event->getReactor() == nullptr) {
            fd_event->setReactor(flconc::Reactor::GetReactor());
        }

        fd_event->setNonBlock();

        ssize_t n = g_sys_write_fun(fd, buf, count);
        if (n > 0) {
            return n;
        }

        toEpoll(fd_event, flconc::IOEvent::WRITE);

        DebugLog << "write func to yield";
        flconc::Coroutine::Yield();

        fd_event->delListenEvents(flconc::IOEvent::WRITE);
        fd_event->clearCoroutine();

        DebugLog << "write func yield back, now to call sys write";
        return g_sys_write_fun(fd, buf, count);

    }

    int connect_hook(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
        DebugLog << "this is hook connect";
        if (flconc::Coroutine::IsMainCoroutine()) {
            DebugLog << "hook disable, call sys connect func";
            return g_sys_connect_fun(sockfd, addr, addrlen);
        }
        flconc::Reactor *reactor = flconc::Reactor::GetReactor();

        flconc::FdEvent::ptr fd_event = flconc::FdEventContainer::GetFdContainer()->getFdEvent(sockfd);
        if (fd_event->getReactor() == nullptr) {
            fd_event->setReactor(reactor);
        }
        flconc::Coroutine *cur_cor = flconc::Coroutine::GetCurrentCoroutine();

        fd_event->setNonBlock();
        int n = g_sys_connect_fun(sockfd, addr, addrlen);
        if (n == 0) {
            DebugLog << "direct connect succ, return";
            return n;
        } else if (errno != EINPROGRESS) {
            DebugLog << "connect error and errno is't EINPROGRESS, errno=" << errno << ",error=" << strerror(errno);
            return n;
        }

        DebugLog << "errno == EINPROGRESS";

        toEpoll(fd_event, flconc::IOEvent::WRITE);

        bool is_timeout = false;

        auto timeout_cb = [&is_timeout, cur_cor]() {

            is_timeout = true;
            flconc::Coroutine::Resume(cur_cor);
        };

        flconc::TimerEvent::ptr event = std::make_shared<flconc::TimerEvent>(
                gRpcConfig->m_max_connect_timeout, false,
                timeout_cb
        );
        flconc::Timer *timer = reactor->getTimer();
        timer->addTimerEvent(event);

        flconc::Coroutine::Yield();

        fd_event->delListenEvents(flconc::IOEvent::WRITE);
        fd_event->clearCoroutine();

        timer->delTimerEvent(event);

        n = g_sys_connect_fun(sockfd, addr, addrlen);
        if ((n < 0 && errno == EISCONN) || n == 0) {
            DebugLog << "connect succ";
            return 0;
        }

        if (is_timeout) {
            ErrorLog << "connect error,  timeout[ " << gRpcConfig->m_max_connect_timeout << "ms]";
            errno = ETIMEDOUT;
        }

        DebugLog << "connect error and errno=" << errno << ", error=" << strerror(errno);
        return -1;

    }

    unsigned int sleep_hook(unsigned int seconds) {

        DebugLog << "this is hook sleep";
        if (flconc::Coroutine::IsMainCoroutine()) {
            DebugLog << "hook disable, call sys sleep func";
            return g_sys_sleep_fun(seconds);
        }

        flconc::Coroutine *cur_cor = flconc::Coroutine::GetCurrentCoroutine();

        bool is_timeout = false;
        auto timeout_cb = [cur_cor, &is_timeout]() {
            DebugLog << "onTime, now resume sleep cor";
            is_timeout = true;

            flconc::Coroutine::Resume(cur_cor);
        };

        flconc::TimerEvent::ptr event = std::make_shared<flconc::TimerEvent>(1000 * seconds, false, timeout_cb);

        flconc::Reactor::GetReactor()->getTimer()->addTimerEvent(event);

        DebugLog << "now to yield sleep";

        while (!is_timeout) {
            flconc::Coroutine::Yield();
        }

        return 0;
    }

}


extern "C" {

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    if (!flconc::g_hook) {
        return g_sys_accept_fun(sockfd, addr, addrlen);
    } else {
        return flconc::accept_hook(sockfd, addr, addrlen);
    }
}

ssize_t read(int fd, void *buf, size_t count) {
    if (!flconc::g_hook) {
        return g_sys_read_fun(fd, buf, count);
    } else {
        return flconc::read_hook(fd, buf, count);
    }
}

ssize_t write(int fd, const void *buf, size_t count) {
    if (!flconc::g_hook) {
        return g_sys_write_fun(fd, buf, count);
    } else {
        return flconc::write_hook(fd, buf, count);
    }
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (!flconc::g_hook) {
        return g_sys_connect_fun(sockfd, addr, addrlen);
    } else {
        return flconc::connect_hook(sockfd, addr, addrlen);
    }
}

unsigned int sleep(unsigned int seconds) {
    if (!flconc::g_hook) {
        return g_sys_sleep_fun(seconds);
    } else {
        return flconc::sleep_hook(seconds);
    }
}

}
