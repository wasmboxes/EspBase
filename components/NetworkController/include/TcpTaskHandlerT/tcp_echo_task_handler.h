#pragma once
namespace NetworkController
{
    
    struct tcp_echo_task_handler_t
    {
        int* id; 
        char* service_name; // null terminated ! '\0'
        int* socket_fd;
    };

    tcp_echo_task_handler_t* tcp_echo_task_handler_hard_copy(tcp_echo_task_handler_t* src);
    void tcp_echo_task_handler_free(tcp_echo_task_handler_t* src);
    tcp_echo_task_handler_t* tcp_echo_task_handler_instantiate();
} // namespace 
