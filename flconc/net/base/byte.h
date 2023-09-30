#pragma once

#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>

namespace flconc {

    int32_t getInt32FromNetByte(const char *buf) {
        int32_t tmp;
        memcpy(&tmp, buf, sizeof(tmp));
        return ntohl(tmp);
    }
}


