#include "NetworkController.h"
#include "PreferencesController.h"


namespace NetworkController{
    int keepAlive = 1;
    int keepIdle = 1;
    int keepInterval = 1;
    int keepCount = 100;
    int on = 1;



    void TcpHandlerEcho(void *pvParameters);
    void TcpHandlerProxy(void *pvParameters);
    void TcpHandlerTransceiver(void *pvParameters);
    void TcpHandlerRawTransceiver(void *pvParameters);
    void TcpHandlerInstantiateTask(tcp_raw_task_handler_t* raw_task_handler, tcp_raw_active_connection* sock);


    void NetworkController::TcpRawTaskHandlerBase(void *pvParameters)
    {
        tcp_raw_task_handler_t* raw_task_handler = (tcp_raw_task_handler_t*)pvParameters;
        
        struct sockaddr_storage dest_addr;
        int* listen_sock = (int*)malloc(sizeof(int));

        pollfd in_poll;
        
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(raw_task_handler->port);

        bool doBreak = false;


        *listen_sock = socket(AF_INET, SOCK_STREAM , IPPROTO_IP);
        if (*listen_sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            vTaskDelete(NULL);
            return;
        }

                        
        struct linger linger;
        linger.l_onoff = 1;
        linger.l_linger = 0;

        int ret = 0;
        ret = setsockopt(*listen_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        // if( ret == 0 ){
        //     ESP_LOGI(TAG, "Set SO_LINGER success with sock : %d", *listen_sock);
        // }
        // else ESP_LOGE(TAG, "Set SO_LINGER failure %d with sock : %d", ret, *listen_sock); 

        // setsockopt(*listen_sock, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
        // ret = setsockopt(*listen_sock, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger) );
        // if( ret == 0 ){
        //     ESP_LOGI(TAG, "Set SO_LINGER success with sock : %d", *listen_sock);
        // }
        // else ESP_LOGE(TAG, "Set SO_LINGER failure %d with sock : %d", ret, *listen_sock); 

        

        ESP_LOGI(TAG, "Socket created");

        int err = bind(*listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            doBreak = true;
        }
        ESP_LOGI(TAG, "Socket bound, port %d", raw_task_handler->port);

        // listen ( <socket fd>, < # of max active sockets > )
        err = listen(*listen_sock, 4);
        if (err != 0) {
            ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
            doBreak = true;
        }


        in_poll.fd = *listen_sock;
        in_poll.events = POLLIN;
        

        while (!doBreak) {

            ESP_LOGI(TAG, "Socket listening");

            int wait_response = 0;
            while(wait_response == 0 && !doBreak){
                // TCP_HANDLER_BASE_POLL:
                wait_response = poll(&in_poll, 1, 100);
                // ERROR
                if(wait_response < 0){
                    ESP_LOGE(TAG, "Poll error %d", wait_response);
                    doBreak = true;
                }
                // TIMEOUT //Check for thread shutdown signal
                else if(wait_response == 0) if(ulTaskNotifyTake(pdTRUE, 10)) doBreak = true;
            }

            if(doBreak) break;

            // Accept Socket
            tcp_raw_active_connection* active_connection = new tcp_raw_active_connection();
            active_connection->xHandle = NULL;
            active_connection->source_addr = (struct sockaddr_storage*)malloc(sizeof(struct sockaddr_storage));
            active_connection->addr_len = (socklen_t*)malloc(sizeof(socklen_t));
            *active_connection->addr_len = sizeof((*active_connection->source_addr));
            active_connection->socket_fd = (int*)malloc(sizeof(int));
            
            *active_connection->socket_fd = accept(*listen_sock, (struct sockaddr *)active_connection->source_addr, active_connection->addr_len);
            ESP_LOGI(TAG, "Active connection : %d, connecton len : %d , with socket fd :  %d", active_connection->source_addr->ss_family, *active_connection->addr_len, *active_connection->socket_fd );

            ESP_LOGI(TAG, "Accepted connection with socket fd :  %d", *active_connection->socket_fd );
            if (*active_connection->socket_fd < 0) {
                ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
                free(active_connection->socket_fd);
                free(active_connection->addr_len);
                free(active_connection->source_addr);
                doBreak = true; // Ovdej se mozda i nemora
            }
            
            if(doBreak) break;
            ret = setsockopt(*active_connection->socket_fd , SOL_SOCKET, SO_LINGER, &linger, sizeof(linger) );
            if( ret == 0 ){
                ESP_LOGI(TAG, "Set SO_LINGER success with sock : %d", *active_connection->socket_fd );
            }
            else ESP_LOGE(TAG, "Set SO_LINGER failure %d with sock : %d", ret, *active_connection->socket_fd ); 
            // setsockopt(*sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
            // setsockopt(*sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
            // setsockopt(*sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
            // setsockopt(*sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
            // setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
            // setsockopt(*sock, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));

            TcpHandlerInstantiateTask(raw_task_handler, active_connection);           

            // Check and free active_connections if handler thread is null ! TODO FREE 
            std::list<tcp_raw_active_connection*>* active_connections = raw_task_handler->active_connections;
            for (std::list<tcp_raw_active_connection*>::iterator it = active_connections->begin(); it != active_connections->end(); ++it){
                std::list<tcp_raw_active_connection*>::iterator prev = it++;
                if((*prev)->xHandle == NULL) close(*(*prev)->socket_fd); //free((*it));
                active_connections->erase(prev);
            }
        }

        
        close(*listen_sock); // close active connections
        ESP_LOGW(TAG, "Deleting active connections");
        std::list<tcp_raw_active_connection*>* active_connections = raw_task_handler->active_connections;
        for (std::list<tcp_raw_active_connection*>::iterator it = active_connections->begin(); it != active_connections->end(); ++it){
            xTaskNotifyGive((*it)->xHandle);
        }
        vTaskDelete(NULL);
    }


    void TcpHandlerInstantiateTask(tcp_raw_task_handler_t* raw_task_handler, tcp_raw_active_connection* active_connection){


            if(raw_task_handler->handler_type == Echo){
                ESP_LOGI(TAG, "TcpRawTaskHandlerBase Instantiating TcpHandlerEcho");

                // stvori se kopija s novim socket fd-om
                tcp_echo_task_handler_t* tcp_echo_task_handler = tcp_echo_task_handler_instantiate();
                tcp_echo_task_handler->id = ((tcp_echo_task_handler_t*)raw_task_handler->handler)->id;
                tcp_echo_task_handler->service_name = ((tcp_echo_task_handler_t*)raw_task_handler->handler)->service_name;
                tcp_echo_task_handler->socket_fd = active_connection->socket_fd;

                // ta kopija se preda i ocekuje se da TcpHandlerEcho oslobodi prostor novo-stvorene kopije, ali ne i memorije na koju pokazuju memberi kopije osim socketa
                xTaskCreate(TcpHandlerEcho, "Echo", X_TASK_STACK_SIZE, (void*)tcp_echo_task_handler, 2, &active_connection->xHandle);
                raw_task_handler->active_connections->push_back(active_connection);
            }
            else if(raw_task_handler->handler_type == Proxy){
                ESP_LOGI(TAG, "TcpRawTaskHandlerBase Instantiating TcpHandlerProxy");

                tcp_proxy_task_handler_t* tcp_proxy_task_handler = tcp_proxy_task_handler_instantiate();
                tcp_proxy_task_handler->dest_addr = ((tcp_proxy_task_handler_t*)raw_task_handler->handler)->dest_addr;
                tcp_proxy_task_handler->id = ((tcp_proxy_task_handler_t*)raw_task_handler->handler)->id;
                tcp_proxy_task_handler->service_name = ((tcp_proxy_task_handler_t*)raw_task_handler->handler)->service_name;
                tcp_proxy_task_handler->socket_fd = active_connection->socket_fd; 

                xTaskCreate(TcpHandlerProxy, "Proxy", X_TASK_STACK_SIZE, (void*)tcp_proxy_task_handler, 4, &active_connection->xHandle);
                raw_task_handler->active_connections->push_back(active_connection);
            }
            else if(raw_task_handler->handler_type == Transceiver){
                ESP_LOGI(TAG, "TcpRawTaskHandlerBase Instantiating TcpHandlerTransceiver");

                tcp_transceiver_task_handler_t* tcp_transceiver_task_handler = tcp_transceiver_task_handler_instantiate();
                tcp_transceiver_task_handler->id = ((tcp_transceiver_task_handler_t*)raw_task_handler->handler)->id;
                tcp_transceiver_task_handler->SocketHandler = ((tcp_transceiver_task_handler_t*)raw_task_handler->handler)->SocketHandler;
                tcp_transceiver_task_handler->service_name = ((tcp_transceiver_task_handler_t*)raw_task_handler->handler)->service_name;
                tcp_transceiver_task_handler->socket_fd = active_connection->socket_fd; 

                xTaskCreate(TcpHandlerTransceiver, "Transceiver", X_TASK_STACK_SIZE, (void*)tcp_transceiver_task_handler, 5, &active_connection->xHandle);
                raw_task_handler->active_connections->push_back(active_connection);
            }
            else if(raw_task_handler->handler_type == RawTransceiver){
                ESP_LOGI(TAG, "TcpRawTaskHandlerBase Instantiating TcpHandlerRawTransceiver");
                // ((tcp_raw_transceiver_task_handler_t*)raw_task_handler->handler)->socket_fd = sock;

                tcp_raw_transceiver_task_handler_t* tcp_raw_transceiver_task_handler = tcp_raw_transceiver_task_handler_instantiate();
                tcp_raw_transceiver_task_handler->id = ((tcp_raw_transceiver_task_handler_t*)raw_task_handler->handler)->id;
                tcp_raw_transceiver_task_handler->SocketHandler = ((tcp_raw_transceiver_task_handler_t*)raw_task_handler->handler)->SocketHandler;
                tcp_raw_transceiver_task_handler->service_name = ((tcp_raw_transceiver_task_handler_t*)raw_task_handler->handler)->service_name;
                
                tcp_raw_transceiver_task_handler->socket_fd = active_connection->socket_fd;     

                ESP_LOGI(TAG, "TcpRawTaskHandlerBase socketAc : %p, %p", &(*active_connection->socket_fd), &(*tcp_raw_transceiver_task_handler->socket_fd));
                ESP_LOGI(TAG, "TcpRawTaskHandlerBase socketAc : %d, %d", *active_connection->socket_fd, *tcp_raw_transceiver_task_handler->socket_fd);

                // TODO pazi na stack size za wasm
                xTaskCreate(TcpHandlerRawTransceiver, "RawTransceiver", X_TASK_STACK_SIZE, (void*)tcp_raw_transceiver_task_handler, 5, &active_connection->xHandle);
                raw_task_handler->active_connections->push_back(active_connection);
            }
            else {
                //free(handle);
            }
    }
}