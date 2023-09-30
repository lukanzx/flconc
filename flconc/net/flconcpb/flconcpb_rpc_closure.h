#pragma once

#include <google/protobuf/stubs/callback.h>
#include <functional>
#include <memory>

namespace flconc {

    class FlconcPbRpcClosure : public google::protobuf::Closure {
    public:
        typedef std::shared_ptr<FlconcPbRpcClosure> ptr;

        explicit FlconcPbRpcClosure(std::function<void()> cb) : m_cb(cb) {

        }

        ~FlconcPbRpcClosure() = default;

        void Run() {
            if (m_cb) {
                m_cb();
            }
        }

    private:
        std::function<void()> m_cb{nullptr};

    };

}

