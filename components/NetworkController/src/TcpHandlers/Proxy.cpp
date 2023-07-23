#include "NetworkController.h"

namespace NetworkController{
    
    void TcpHandlerProxy(void *pvParameters){
        tcp_proxy_task_handler_t* task_handler = (tcp_proxy_task_handler_t*)pvParameters;
        
        int err;
        int timeout = NETWORK_CONTROLLER_TCP_PROXY_TIMEOUT_MS;
        bool doBreak = false;
        
        int in_len;
        char in_rx_buffer[NETWORK_CONTROLLER_BUFFER_MAX_LEN];

        int out_sock;
        int out_len;
        char out_rx_buffer[NETWORK_CONTROLLER_BUFFER_MAX_LEN];

        ESP_LOGI(TAG, "New socket task with socket fd : %d", *task_handler->socket_fd);

        out_sock =  socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (out_sock < 0) {
            ESP_LOGE(TAG, "Unable to create proxy socket: errno %d", errno);
            doBreak = true;
        }
        ESP_LOGI(TAG, "Socket created, proxy connecting ");
        
        err = connect(out_sock, (struct sockaddr *)&(task_handler->dest_addr), sizeof(struct sockaddr_in6));
        if (err != 0) {
            ESP_LOGE(TAG, "Proxy socket unable to connect: errno %d", errno);
            doBreak = true;
        }
        
        ESP_LOGI(TAG, "Proxy Successfully connected"); 

        pollfd in_poll;
        pollfd out_poll;

        in_poll.fd = *task_handler->socket_fd;
        in_poll.events = POLLIN;

        out_poll.fd = out_sock;
        out_poll.events = POLLIN;

        int in_ret;
        int out_ret;

        while (timeout > 0 && !doBreak){

            in_ret = poll(&in_poll, 1, 10); 

            switch (in_ret) {
                case -1:
                    // Error
                    doBreak = true;
                    break;
                case 0:
                    // Timeout 
                    if(ulTaskNotifyTake(pdTRUE, 10)) doBreak = true;
                    timeout--;
                    break;
                default:
                    in_len = recv(*task_handler->socket_fd,in_rx_buffer,sizeof(in_rx_buffer)-1, 0);
                    if (in_len < 0) {
                        ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
                        doBreak = true;
                    } else if (in_len == 0) {
                        ESP_LOGW(TAG, "Connection closed");
                        doBreak = true;
                    } else {
                        err = send(out_sock, in_rx_buffer, in_len * sizeof(char), 0);
                        if (err < 0) {
                            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                            doBreak = true;
                        }
                        timeout = NETWORK_CONTROLLER_TCP_PROXY_TIMEOUT_MS;
                    }
                    break;
            }

            if(doBreak) break;

            out_ret = poll(&out_poll, 1, 10); 

            switch (out_ret) {
                case -1:
                    // Error
                    doBreak = true;
                    break;
                case 0:
                    // Timeout 
                    if(ulTaskNotifyTake(pdTRUE, 10)) doBreak = true;
                    timeout--;
                    break;
                default:
                    out_len = recv(out_sock,out_rx_buffer,sizeof(out_rx_buffer), 0);
                    if (out_len < 0) {
                        ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
                        doBreak = true;
                    } else if (out_len == 0) {
                        ESP_LOGW(TAG, "Connection closed");
                        doBreak = true;
                    } else {
                        err = send(*task_handler->socket_fd, out_rx_buffer, out_len * sizeof(char), 0);
                        if (err < 0) {
                            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                            doBreak = true;
                        }
                        timeout = NETWORK_CONTROLLER_TCP_PROXY_TIMEOUT_MS/10;
                    }
                    break;
            }


        }
        // TCP_HANDLER_PROXY_CLEANUP:
        ESP_LOGW(TAG, "ENDING PROXY TASK...");
        close(out_sock);
        close(*task_handler->socket_fd);
        free(task_handler);
        vTaskDelete(NULL);
    }
}