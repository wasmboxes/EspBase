#include "NetworkController.h"

namespace NetworkController{
    void TcpHandlerRawTransceiver(void *pvParameters)
    {
        tcp_raw_transceiver_task_handler_t* task_handler = (tcp_raw_transceiver_task_handler_t*)pvParameters;
        tcp_raw_transceiver_task_context_t* context = new tcp_raw_transceiver_task_context_t;
        context->socket_fd = task_handler->socket_fd;
        ESP_LOGE(TAG, "Calling operator () TcpHandlerRawTransceiver with sock : %d", *task_handler->socket_fd);
        TcpTranscieverResponse resp;
        bool doBreak = false;

        do{
            resp = task_handler->SocketHandler->operator()(context);
            ESP_LOGI(TAG, "Calling operator () TcpHandlerRawTransceiver with sock : %d", *task_handler->socket_fd);
            switch (resp)
            {
            case TcpTranscieverResponse::Read :
                break;
            case TcpTranscieverResponse::Close :
            default:
                doBreak = true;
                break;
            }
            if(doBreak) break;
            if(ulTaskNotifyTake(pdTRUE, 10)) doBreak = true;
        }while(!doBreak); 

        // TCP_HANDLER_RAW_TRANSCEIVER_CLEANUP:
        ESP_LOGE(TAG, "ENDING RAW TRANSCEIVER TASK... with sock : %d", *task_handler->socket_fd);

       

        close(*task_handler->socket_fd);
        //free(task_handler);
        vTaskDelete(NULL);
    }
} // namespace 
