#include "NetworkController.h"

namespace NetworkController{
    void TcpHandlerTransceiver(void *pvParameters)
    {
        tcp_transceiver_task_handler_t* task_handler = (tcp_transceiver_task_handler_t*)pvParameters;
        
        int len;
        char rx_buffer[NETWORK_CONTROLLER_BUFFER_MAX_LEN];
        ESP_LOGI(TAG, "New socket task with socket fd : %d", *task_handler->socket_fd);

        pollfd in_poll;
        int in_ret;
        in_poll.fd = *task_handler->socket_fd;
        in_poll.events = POLLIN;

        bool doBreak = false;

        do {
            
            // TCP_HANDLER_TRANSCEIVER_POLL:
            do{
                in_ret = poll(&in_poll, 1, 10); 
                switch (in_ret) {
                    case -1: // Error
                        doBreak = true;
                    case 0: // Timeout
                        if(ulTaskNotifyTake(pdTRUE, 10)) doBreak = true;
                }
            }while(in_ret == 0 && !doBreak);

            len = recv(*task_handler->socket_fd, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len < 0) {
                ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
                break;
            } else if (len == 0) {
                ESP_LOGW(TAG, "Connection closed");
                break;
            } else {
                rx_buffer[len] = 0; // Null-terminate whatever is received and treat it like a string

                ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer);
                // ESP_LOGI(TAG, "Free Heap Before Handler Named : %s = %d ", task_handler->service_name, esp_get_free_heap_size());

                tcp_transceiver_task_context_t* context = (tcp_transceiver_task_context_t*)malloc(sizeof(tcp_transceiver_task_context_t)); 
                context->inBytes = rx_buffer;
                context->inLen = &len;
                
                ESP_LOGI(TAG, "Running Handler Named : %s", task_handler->service_name);
                // bool response = (*(task_handler->SocketHandler))(context);

                TcpTranscieverResponse response = task_handler->SocketHandler->operator()(context);

                // ESP_LOGI(TAG, "Output for Handler Named : %s = %d ", task_handler->service_name, response);
                // send() can return less bytes than supplied length.
                // Walk-around for robust implementation.
                if(response == TcpTranscieverResponse::Write || response == TcpTranscieverResponse::WriteAndClose){
                    int to_write = *(context->outLen);
                    while (to_write > 0) {
                        int written = send(*task_handler->socket_fd, context->outBytes + *(context->outLen) - to_write , to_write, 0);
                        if (written < 0) {
                            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                            doBreak = true;
                            break;
                        }
                        to_write -= written;
                    }
                }
                // FREE CONTEXT OUTPUT, input je samo pointer na glavni buffer
                free(context->outBytes);
                free(context->outLen);

                // ESP_LOGI(TAG, "Free Heap After Handler Named : %s = %d ", task_handler->service_name, esp_get_free_heap_size());
                if(response == TcpTranscieverResponse::Close || response == TcpTranscieverResponse::WriteAndClose){
                    ESP_LOGW(TAG, "Handler return true, closing socket");
                    break;
                }
            }
        } while (!doBreak);

        // TCP_HANDLER_TRANSCEIVER_CLEANUP:
        ESP_LOGW(TAG, "ENDING TRANSCEIVER TASK...");
        close(*task_handler->socket_fd);
        free(task_handler);
        vTaskDelete(NULL);
    }
}