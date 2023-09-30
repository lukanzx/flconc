#pragma once

namespace flconc {

#ifndef SYS_ERROR_PREFIX
#define SYS_ERROR_PREFIX(xx) 1000##xx
#endif

    const int ERROR_PEER_CLOSED = SYS_ERROR_PREFIX(0000);
    const int ERROR_FAILED_CONNECT = SYS_ERROR_PREFIX(0001);
    const int ERROR_FAILED_GET_REPLY = SYS_ERROR_PREFIX(0002);
    const int ERROR_FAILED_DESERIALIZE = SYS_ERROR_PREFIX(0003);
    const int ERROR_FAILED_SERIALIZE = SYS_ERROR_PREFIX(0004);

    const int ERROR_FAILED_ENCODE = SYS_ERROR_PREFIX(0005);
    const int ERROR_FAILED_DECODE = SYS_ERROR_PREFIX(0006);

    const int ERROR_RPC_CALL_TIMEOUT = SYS_ERROR_PREFIX(0007);

    const int ERROR_SERVICE_NOT_FOUND = SYS_ERROR_PREFIX(0008);

    const int ERROR_METHOD_NOT_FOUND = SYS_ERROR_PREFIX(0009);

    const int ERROR_PARSE_SERVICE_NAME = SYS_ERROR_PREFIX(0010);
    const int ERROR_NOT_SET_ASYNC_PRE_CALL = SYS_ERROR_PREFIX(
            0011);
    const int ERROR_CONNECT_SYS_ERR = SYS_ERROR_PREFIX(0012);

} 
