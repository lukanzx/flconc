#pragma once

#include <stdint.h>

#include "abstract_codec.h"
#include "abstract_data.h"
#include "flconcpb_data.h"

namespace flconc {

    class FlconcPbCodeC : public AbstractCodeC {
    public:

        FlconcPbCodeC();

        ~FlconcPbCodeC();

        void encode(TcpBuffer *buf, AbstractData *data);

        void decode(TcpBuffer *buf, AbstractData *data);

        virtual ProtocalType getProtocalType();

        const char *encodePbData(FlconcPbStruct *data, int &len);

    };

} 

