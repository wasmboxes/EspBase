#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "NetworkController.h"
#include "PreferencesController.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "TcpTaskHandlerT.h"


namespace NetworkController{

    esp_err_t NetworkController::TcpAddHandler(void* handler, TcpHandlerType_t type){ // to map
        switch (type)
        {
        case Echo:
                this->TcpConfigurationTypes[*((tcp_echo_task_handler_t*)handler)->id] = type;
                this->TcpConfigurations[*((tcp_echo_task_handler_t*)handler)->id] = handler;
                ESP_LOGI(TAG,"TCP added echo task handler : %d", *((tcp_echo_task_handler_t*)handler)->id);
            break;
        case Transceiver:
                this->TcpConfigurationTypes[*((tcp_echo_task_handler_t*)handler)->id] = type;
                this->TcpConfigurations[*((tcp_echo_task_handler_t*)handler)->id] = handler;
                ESP_LOGI(TAG,"TCP added tr task handler : %d", *((tcp_echo_task_handler_t*)handler)->id);
        break;
        case RawTransceiver:
                this->TcpConfigurationTypes[*((tcp_echo_task_handler_t*)handler)->id] = type;
                this->TcpConfigurations[*((tcp_echo_task_handler_t*)handler)->id] = handler;
                ESP_LOGI(TAG,"TCP added raw tr task handler : %d", *((tcp_echo_task_handler_t*)handler)->id);
        break;
        case Proxy:
                this->TcpConfigurationTypes[*((tcp_echo_task_handler_t*)handler)->id] = type;
                this->TcpConfigurations[*((tcp_echo_task_handler_t*)handler)->id] = handler;
                ESP_LOGI(TAG,"TCP added proxy task handler : %d", *((tcp_echo_task_handler_t*)handler)->id);
        break;
        default:
            return ESP_ERR_INVALID_ARG;
        }
        return ESP_OK;
    }

    esp_err_t NetworkController::TcpRemoveHandler(int handler_id){ // to map
        this->TcpConfigurations.erase(handler_id);
        ESP_LOGI(TAG,"TCP removed handler : %d", handler_id);
        return ESP_OK;
    }

    // check if TcpActiveHandlerToPortNumberMap exists
    // check if TcpRawConfigurations for port number exists
    // call kill raw handler for TcpRawConfigurations handler id
    esp_err_t NetworkController::TcpKillHandler(int handler_id){
        if(this->TcpActiveHandlerIdToPortNumberMap.find(handler_id) == this->TcpActiveHandlerIdToPortNumberMap.end()){
            ESP_LOGI(TAG,"TCP error handler id %d doenst exist", handler_id);
            return ESP_ERR_INVALID_ARG; // handler doesnt have port number , not active or programmer error
        }
        // port exists, handler exists, raw handler exists, Kill raw handler
        else{
            this->TcpKillRawHandlerForPort(this->TcpActiveHandlerIdToPortNumberMap[handler_id]);
            this->TcpActiveHandlerIdToPortNumberMap.erase(handler_id);
            ESP_LOGI(TAG,"TCP killed handler : %d", handler_id);
            return ESP_OK;
        }
    }

    int  NetworkController::TcpBindHandlerToPort(int handler_id){
        ESP_LOGI(TAG,"TCP binding handler %d ", handler_id);
        this->TcpInstantiateRawHandlerForHandler(handler_id);
        return this->TcpActiveHandlerIdToPortNumberMap[handler_id];
    }
    bool NetworkController::TcpBindHandlerToPort(int handler_id, int port){
        ESP_LOGI(TAG,"TCP binding handler %d to port : %d", handler_id, port);
        this->TcpInstantiateRawHandlerForHandler(handler_id, port);
        return true;
    }

    int NetworkController::TcpGetPortForHandlerId(int handler_id){
        if(this->TcpActiveHandlerIdToPortNumberMap.find(handler_id) == this->TcpActiveHandlerIdToPortNumberMap.end()){
            return -1;
        }
        else return this->TcpActiveHandlerIdToPortNumberMap[handler_id];
    }

    esp_err_t NetworkController::TcpInstantiateRawHandlerForHandler(int handler_id, int port){
        
        if(this->TcpConfigurations.find(handler_id) == this->TcpConfigurations.end()){
            ESP_LOGI(TAG,"TCP instantiate error, handler %d doenst exist", handler_id);
            return ESP_ERR_INVALID_STATE;
        }

        this->TcpRegisterPortNumberAllocation(port);
        tcp_raw_task_handler_t* raw_handler = tcp_raw_task_handler_instantiate();
        raw_handler->port = port;
        raw_handler->handler = this->TcpConfigurations[handler_id];
        raw_handler->handler_type = this->TcpConfigurationTypes[handler_id];

        this->TcpActiveHandlerIdToPortNumberMap[handler_id] = raw_handler->port;
        this->TcpRawConfigurations[raw_handler->port] = raw_handler;

        ESP_LOGI(TAG,"TCP starting xtask for handler %d for allocated port : %d", handler_id, port);
        xTaskCreate(this->TcpRawTaskHandlerBase, "tcp_server", X_TASK_STACK_SIZE, (void*)raw_handler, 5, &raw_handler->xHandle);

        return ESP_OK;
    }

    esp_err_t NetworkController::TcpInstantiateRawHandlerForHandler(int handler_id){
        return this->TcpInstantiateRawHandlerForHandler(handler_id, this->TcpAllocatePortNumber());
    }

    esp_err_t NetworkController::TcpKillRawHandlerForPort(int port){
            if(this->TcpRawConfigurations.find(port) == this->TcpRawConfigurations.end())
            {
                ESP_LOGI(TAG,"TCP error raw handler for port %d doenst exist", port);
                return ESP_ERR_INVALID_ARG; // raw handler not found for port
            }
            // Notify Base to kill all connections and close socket
            xTaskNotifyGive(this->TcpRawConfigurations[port]->xHandle);
            ESP_LOGI(TAG,"TCP deleted raw handler and xtask for port %d", port);          
            // delete this->TcpRawConfigurations[port]; // TODO !! check !!
            
            vTaskDelay(100 / portTICK_PERIOD_MS); // wait 100ms before deleting handler
            tcp_raw_task_handler_free_recursive(this->TcpRawConfigurations[port]);

            this->TcpRawConfigurations.erase(port);
            return ESP_OK;
    }

    // ! NEOVISNO o tcp_task_handler, KORISTI PAZLJIVO ! nema dealokacije       
    esp_err_t NetworkController::TcpBindRawHandlerToPort(tcp_raw_task_handler_t* raw_handler){
        this->TcpRawConfigurations[raw_handler->port] = raw_handler;
        ESP_LOGI(TAG,"TCP starting xtask for raw handler for allocated port : %d", raw_handler->port);
        xTaskCreate(this->TcpRawTaskHandlerBase, "tcp_server", X_TASK_STACK_SIZE, (void*)raw_handler, 10, &raw_handler->xHandle);
        return ESP_OK;
    }


    int NetworkController::TcpAllocatePortNumber(){
        this->rollingPortNum += 1; // TODO, check map for open ports
        int port = this->rollingPortNum + this->startingPortNum;
        this->TcpRegisterPortNumberAllocation(port);
        ESP_LOGI(TAG,"TCP allocated port : %d", port);
        return port;
    }
    void NetworkController::TcpDellocatePortNumber(int port){
        return; // TODO
    }

    int NetworkController::TcpRegisterPortNumberAllocation(int port){
        return 1000; // TODO
    }


}