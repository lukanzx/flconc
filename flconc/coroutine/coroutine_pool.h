#pragma once

#include <vector>

#include "coroutine.h"
#include "mutex.h"
#include "memory.h"

namespace flconc {

    class CoroutinePool {

    public:
        CoroutinePool(int pool_size, int stack_size = 1024 * 128);

        ~CoroutinePool();

        Coroutine::ptr getCoroutineInstanse();

        void returnCoroutine(Coroutine::ptr cor);

    private:
        int m_pool_size{0};
        int m_stack_size{0};

        std::vector<std::pair<Coroutine::ptr, bool>> m_free_cors;

        Mutex m_mutex;

        std::vector<Memory::ptr> m_memory_pool;
    };

    CoroutinePool *GetCoroutinePool();

}

