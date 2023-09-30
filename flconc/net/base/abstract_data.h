#pragma once

namespace flconc {

    class AbstractData {
    public:
        AbstractData() = default;
        virtual ~AbstractData() {};
        bool decode_succ{false};
        bool encode_succ{false};
    };

}

