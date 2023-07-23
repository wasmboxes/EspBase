#pragma once
#include "lwip/sockets.h"
namespace NetworkController
{
    struct tcp_proxy_task_handler_t
    {
        int* id; 
        char* service_name; // null terminated ! '\0'
        sockaddr_in dest_addr;
        int* socket_fd;
    };

    tcp_proxy_task_handler_t* tcp_proxy_task_handler_hard_copy(tcp_proxy_task_handler_t* src);
    void tcp_proxy_task_handler_free(tcp_proxy_task_handler_t* src);
    tcp_proxy_task_handler_t* tcp_proxy_task_handler_instantiate();
} // namespace 
