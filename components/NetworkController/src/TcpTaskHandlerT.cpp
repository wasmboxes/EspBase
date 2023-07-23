#include <stdio.h>
#include <stdlib.h>
#include "tcp_transceiver_task_handler.h"
#include "tcp_echo_task_handler.h"
#include "tcp_proxy_task_handler.h"
#include "tcp_raw_transceiver_task_handler.h"
#include "TcpTaskHandlerT.h"

namespace NetworkController
{
    
    void tcp_raw_task_handler_free_recursive(tcp_raw_task_handler_t* src){
        switch (src->handler_type)
        {
        case Echo:
            tcp_echo_task_handler_free((tcp_echo_task_handler_t*)src->handler);
            break;
        case Proxy:
            tcp_proxy_task_handler_free((tcp_proxy_task_handler_t*)src->handler);\
            break;
        case Transceiver:
            tcp_transceiver_task_handler_free((tcp_transceiver_task_handler_t*)src->handler);
        case RawTransceiver:
            tcp_raw_transceiver_task_handler_free((tcp_raw_transceiver_task_handler_t*)src->handler);
        case None:
            break;
        }
        // free(src->xHandle);
    }
    tcp_raw_task_handler_t* tcp_raw_task_handler_instantiate(){
        tcp_raw_task_handler_t* dest = (tcp_raw_task_handler_t*)malloc(sizeof(tcp_raw_task_handler_t));
        dest->handler = nullptr;
        dest->handler_type = None;
        dest->active_connections = new std::list<tcp_raw_active_connection*>;
        return dest;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tcp_transceiver_task_handler_t* tcp_transceiver_task_handler_hard_copy(tcp_transceiver_task_handler_t* src){
        tcp_transceiver_task_handler_t* dest = (tcp_transceiver_task_handler_t*)malloc(sizeof(tcp_transceiver_task_handler_t));
        dest->id = (int*)malloc(sizeof(int));
        dest->id = src->id;
        dest->socket_fd = (int*)malloc(sizeof(int));
        dest->socket_fd = src->socket_fd;
        dest->SocketHandler = src->SocketHandler;
        return dest;
    }
    void tcp_transceiver_task_handler_free(tcp_transceiver_task_handler_t* src){
        free(src->id);
        // free(src->socket_fd);
        free(src);
    }
    tcp_transceiver_task_handler_t* tcp_transceiver_task_handler_instantiate(){
        tcp_transceiver_task_handler_t* dest = (tcp_transceiver_task_handler_t*)malloc(sizeof(tcp_transceiver_task_handler_t));
        return dest;
    }

    TcpTranscieverResponse TcpTranscieverBase::operator()(tcp_transceiver_task_context_t* a){
        return TcpTranscieverResponse::Close;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tcp_proxy_task_handler_t* tcp_proxy_task_handler_hard_copy(tcp_proxy_task_handler_t* src){
        tcp_proxy_task_handler_t* dest = (tcp_proxy_task_handler_t*)malloc(sizeof(tcp_proxy_task_handler_t));
        dest->id = (int*)malloc(sizeof(int));
        dest->id = src->id;
        dest->socket_fd = (int*)malloc(sizeof(int));
        dest->socket_fd = src->socket_fd;
        return dest;
    }
    void tcp_proxy_task_handler_free(tcp_proxy_task_handler_t* src){
        free(src->id);
        // free(src->socket_fd);
        free(src);
    }
    tcp_proxy_task_handler_t* tcp_proxy_task_handler_instantiate(){
        tcp_proxy_task_handler_t* dest = (tcp_proxy_task_handler_t*)malloc(sizeof(tcp_proxy_task_handler_t));
        return dest;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tcp_echo_task_handler_t* tcp_echo_task_handler_hard_copy(tcp_echo_task_handler_t* src){
        tcp_echo_task_handler_t* dest = (tcp_echo_task_handler_t*)malloc(sizeof(tcp_echo_task_handler_t));
        dest->id = (int*)malloc(sizeof(int));
        dest->id = src->id;
        dest->socket_fd = (int*)malloc(sizeof(int));
        dest->socket_fd = src->socket_fd;
        return dest;
    }
    void tcp_echo_task_handler_free(tcp_echo_task_handler_t* src){
        free(src->id);
        //free(src->socket_fd);
        free(src);
    }
    tcp_echo_task_handler_t* tcp_echo_task_handler_instantiate(){
        tcp_echo_task_handler_t* dest = (tcp_echo_task_handler_t*)malloc(sizeof(tcp_echo_task_handler_t));
        return dest;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tcp_raw_transceiver_task_handler_t* tcp_raw_transceiver_task_handler_hard_copy(tcp_raw_transceiver_task_handler_t* src){
        tcp_raw_transceiver_task_handler_t* dest = (tcp_raw_transceiver_task_handler_t*)malloc(sizeof(tcp_raw_transceiver_task_handler_t));
        dest->id = (int*)malloc(sizeof(int));
        dest->id = src->id;
        dest->socket_fd = (int*)malloc(sizeof(int));
        dest->socket_fd = src->socket_fd;
        return dest;        
    }
    void tcp_raw_transceiver_task_handler_free(tcp_raw_transceiver_task_handler_t* src){
        free(src->id);
        // free(src->socket_fd);
        free(src);
    }
    tcp_raw_transceiver_task_handler_t* tcp_raw_transceiver_task_handler_instantiate(){
        tcp_raw_transceiver_task_handler_t* dest = (tcp_raw_transceiver_task_handler_t*)malloc(sizeof(tcp_raw_transceiver_task_handler_t));
        return dest;
    }

    TcpTranscieverResponse TcpRawTranscieverBase::operator()(tcp_raw_transceiver_task_context_t* a){
        return TcpTranscieverResponse::Close;
    }
}