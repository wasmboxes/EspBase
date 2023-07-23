#pragma once
namespace NetworkController
{
    enum class TcpTranscieverResponse{
        Close, // closes socket without reading
        Read,  // continues the loop
        Write, // write outBytes
        WriteAndClose, //
        None
    };

    struct tcp_transceiver_task_context_t{
        char* inBytes;
        int* inLen;
        char* outBytes;
        int* outLen;
        int* socket_fd;
    };

    class TcpTranscieverBase{
            public:
            virtual TcpTranscieverResponse operator()(tcp_transceiver_task_context_t* a);
    };
    
    struct tcp_transceiver_task_handler_t
    {
        int* id; 
        char* service_name; // null terminated ! '\0'
        // caller is responsible for outBytes and context deallocation, 
        // Sockethandler function is responsible for outBytes allocation
        // true - close socket, end transmission, false - keep socket open
        //bool (*SocketHandler) (tcp_transceiver_task_context_t* context);
        TcpTranscieverBase* SocketHandler;
        int* socket_fd;
    };

    tcp_transceiver_task_handler_t* tcp_transceiver_task_handler_hard_copy(tcp_transceiver_task_handler_t* src);
    void tcp_transceiver_task_handler_free(tcp_transceiver_task_handler_t* src);
    tcp_transceiver_task_handler_t* tcp_transceiver_task_handler_instantiate();
} // namespace 
