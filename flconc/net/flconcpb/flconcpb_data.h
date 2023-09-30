#pragma once

#include <stdint.h>
#include <vector>
#include <string>

#include "abstract_data.h"
#include "log.h"

namespace flconc {

    class FlconcPbStruct : public AbstractData {
    public:
        typedef std::shared_ptr<FlconcPbStruct> pb_ptr;

        FlconcPbStruct() = default;

        ~FlconcPbStruct() = default;

        FlconcPbStruct(const FlconcPbStruct &) = default;

        FlconcPbStruct &operator=(const FlconcPbStruct &) = default;

        FlconcPbStruct(FlconcPbStruct &&) = default;

        FlconcPbStruct &operator=(FlconcPbStruct &&) = default;

        /*
        **  min of package is: 1 + 4 + 4 + 4 + 4 + 4 + 4 + 1 = 26 bytes
        **
        */
        int32_t pk_len{0};
        int32_t msg_req_len{0};
        std::string msg_req;
        int32_t service_name_len{0};
        std::string service_full_name;
        int32_t err_code{
                0};
        int32_t err_info_len{0};
        std::string err_info;
        std::string pb_data;
        int32_t check_num{-1};

    };

}


