#pragma once
namespace NetworkController
{
    
    struct tcp_raw_transceiver_task_context_t{
        int* socket_fd;
    };

    class TcpRawTranscieverBase{
            public:
            virtual TcpTranscieverResponse operator()(tcp_raw_transceiver_task_context_t* a);
    };

    struct tcp_raw_transceiver_task_handler_t
    {
        int* id; 
        char* service_name; // null terminated ! '\0'
        TcpRawTranscieverBase* SocketHandler;
        int* socket_fd;
    };

    tcp_raw_transceiver_task_handler_t* tcp_raw_transceiver_task_handler_hard_copy(tcp_raw_transceiver_task_handler_t* src);
    void tcp_raw_transceiver_task_handler_free(tcp_raw_transceiver_task_handler_t* src);
    tcp_raw_transceiver_task_handler_t* tcp_raw_transceiver_task_handler_instantiate();
} // namespace 
