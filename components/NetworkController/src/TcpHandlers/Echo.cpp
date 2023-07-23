#include "NetworkController.h"
namespace NetworkController{
    
    void TcpHandlerEcho(void *pvParameters)
    {
        tcp_echo_task_handler_t* task_handler = (tcp_echo_task_handler_t*)pvParameters;

        int len;
        char rx_buffer[NETWORK_CONTROLLER_BUFFER_MAX_LEN];
        ESP_LOGI(TAG, "New socket task with socket fd : %d", *task_handler->socket_fd);

        pollfd in_poll;
        int in_ret = 0;
        in_poll.fd = *task_handler->socket_fd;
        in_poll.events = POLLIN;
        bool doBreak = false;

        while(!doBreak){
            ESP_LOGI(TAG, "POLL SOCKET FD : %d", *task_handler->socket_fd);

            // TCP_HANDLER_ECHO_POLL:
            while(in_ret == 0 && !doBreak){
                in_ret = poll(&in_poll, 1, 1000); 
                switch (in_ret) {
                    case -1: // Error
                        ESP_LOGE(TAG, "POLL SOCKET FD : %d ERROR", *task_handler->socket_fd);
                        doBreak = true;
                    case 0: // Timeout
                        ESP_LOGI(TAG, "POLL SOCKET FD : %d TIMEOUT", *task_handler->socket_fd);
                        if(ulTaskNotifyTake(pdTRUE, 10)) doBreak = true;
                }
            }

            if(doBreak) break;
           
            // Else, got data to read
            len = recv(*task_handler->socket_fd, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len < 0) {
                ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
                break;
            } else if (len == 0) {
                ESP_LOGW(TAG, "Connection closed");
            } else {
                rx_buffer[len] = 0; // Null-terminate whatever is received and treat it like a string
                ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer);

                // send() can return less bytes than supplied length.
                // Walk-around for robust implementation.
                int to_write = len;
                while (to_write > 0) {
                    int written = send(*task_handler->socket_fd, rx_buffer + (len - to_write), to_write, 0);
                    if (written < 0) {
                        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                        doBreak = true;
                        break;
                    }
                    to_write -= written;
                }
            }
        };
        // TCP_HANDLER_ECHO_CLEANUP:
        ESP_LOGW(TAG, "ENDING ECHO TASK...");

        close(*task_handler->socket_fd);
        free(task_handler);
        vTaskDelete(NULL);
    }
} // namespace 
