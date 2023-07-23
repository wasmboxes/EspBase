#pragma once
#include "lwip/sockets.h"
#include <list>

#include "./TcpTaskHandlerT/tcp_transceiver_task_handler.h"
#include "./TcpTaskHandlerT/tcp_raw_transceiver_task_handler.h"
#include "./TcpTaskHandlerT/tcp_echo_task_handler.h"
#include "./TcpTaskHandlerT/tcp_proxy_task_handler.h"

namespace NetworkController{
    #define MAX_ACIVE_CONNECTIONS 20
    #define X_TASK_STACK_SIZE 8192 * 2

    

    enum TcpHandlerType_t{
        None,
        Echo,           // Echo server
        Transceiver,    // Context with in/out bytes
        RawTransceiver, // handle raw socket_fd
        Proxy           // proxy to ip/port
    };

    struct tcp_raw_active_connection{
        TaskHandle_t xHandle;
        int* socket_fd;
        struct sockaddr_storage* source_addr;
        socklen_t* addr_len;
    };

    struct tcp_raw_task_handler_t{
        int port;
        TaskHandle_t xHandle;
        TcpHandlerType_t handler_type;
        void* handler;
        std::list<tcp_raw_active_connection*>* active_connections;
        // void** active_connections;
        // int active_connections_len;
    };

    void tcp_raw_task_handler_free_recursive(tcp_raw_task_handler_t* src);
    tcp_raw_task_handler_t* tcp_raw_task_handler_instantiate();

}