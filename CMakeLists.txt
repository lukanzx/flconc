cmake_minimum_required(VERSION 3.22.1)
project(Lib-flconc)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -std=c++11 -Wall -Wno-deprecated -Wno-unused-but-set-variable")

include_directories(/usr/local/include)
include_directories(/usr/include)

include_directories(flconc/)
include_directories(flconc/comm/)
include_directories(flconc/coroutine/)
include_directories(flconc/net/base/)
include_directories(flconc/net/flconcpb/)
include_directories(flconc/net/http/)
include_directories(flconc/net/tcp/)

add_library(flconc SHARED)

target_sources(flconc PUBLIC
        flconc/comm/config.cc
        flconc/comm/config.h
        flconc/comm/error_code.h
        flconc/comm/log.cc
        flconc/comm/log.h
        flconc/comm/msg_req.cc
        flconc/comm/msg_req.h
        flconc/comm/mysql_instase.cc
        flconc/comm/mysql_instase.h
        flconc/comm/run_time.h
        flconc/comm/start.cc
        flconc/comm/start.h
        flconc/comm/string_util.cc
        flconc/comm/string_util.h
        flconc/comm/thread_pool.cc
        flconc/comm/thread_pool.h
)

target_sources(flconc PUBLIC
        flconc/coroutine/coctx.h
        flconc/coroutine/coroutine_hook.cc
        flconc/coroutine/coroutine_hook.h
        flconc/coroutine/coroutine_pool.cc
        flconc/coroutine/coroutine_pool.h
        flconc/coroutine/coroutine.cc
        flconc/coroutine/coroutine.h
        flconc/coroutine/memory.cc
        flconc/coroutine/memory.h
)

target_sources(flconc PUBLIC
        flconc/net/base/abstract_codec.h
        flconc/net/base/abstract_data.h
        flconc/net/base/abstract_dispatcher.h
        flconc/net/base/byte.h
        flconc/net/base/fd_event.cc
        flconc/net/base/fd_event.h
        flconc/net/base/mutex.cc
        flconc/net/base/mutex.h
        flconc/net/base/net_address.cc
        flconc/net/base/net_address.h
        flconc/net/base/reactor.cc
        flconc/net/base/reactor.h
        flconc/net/base/timer.cc
        flconc/net/base/timer.h
)

target_sources(flconc PUBLIC
        flconc/net/flconcpb/flconcpb_codec.cc
        flconc/net/flconcpb/flconcpb_codec.h
        flconc/net/flconcpb/flconcpb_data.h
        flconc/net/flconcpb/flconcpb_rpc_async_channel.cc
        flconc/net/flconcpb/flconcpb_rpc_async_channel.h
        flconc/net/flconcpb/flconcpb_rpc_channel.cc
        flconc/net/flconcpb/flconcpb_rpc_channel.h
        flconc/net/flconcpb/flconcpb_rpc_closure.h
        flconc/net/flconcpb/flconcpb_rpc_controller.cc
        flconc/net/flconcpb/flconcpb_rpc_controller.h
        flconc/net/flconcpb/flconcpb_rpc_dispatcher.cc
        flconc/net/flconcpb/flconcpb_rpc_dispatcher.h

)

target_sources(flconc PUBLIC
        flconc/net/http/http_codec.cc
        flconc/net/http/http_codec.h
        flconc/net/http/http_define.cc
        flconc/net/http/http_define.h
        flconc/net/http/http_dispatcher.cc
        flconc/net/http/http_dispatcher.h
        flconc/net/http/http_request.h
        flconc/net/http/http_response.h
        flconc/net/http/http_servlet.cc
        flconc/net/http/http_servlet.h
)

target_sources(flconc PUBLIC
        flconc/net/tcp/abstract_slot.h
        flconc/net/tcp/io_thread.cc
        flconc/net/tcp/io_thread.h
        flconc/net/tcp/tcp_buffer.cc
        flconc/net/tcp/tcp_buffer.h
        flconc/net/tcp/tcp_client.cc
        flconc/net/tcp/tcp_client.h
        flconc/net/tcp/tcp_connection_time_wheel.cc
        flconc/net/tcp/tcp_connection_time_wheel.h
        flconc/net/tcp/tcp_connection.cc
        flconc/net/tcp/tcp_connection.h
        flconc/net/tcp/tcp_server.cc
        flconc/net/tcp/tcp_server.h
)

set(LIB_LOAD "/usr/lib/x86_64-linux-gnu")

target_link_libraries(flconc
        ${LIB_LOAD}/libtinyxml.so
        /usr/local/lib/libprotobuf.so
        ${LIB_LOAD}/libmysqlclient.so
)

set_target_properties(flconc PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
)

