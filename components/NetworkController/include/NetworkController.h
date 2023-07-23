#pragma once
#include "PreferencesController.h"
#include <stdio.h>
#include "esp_err.h"
#include "PreferencesController.h"
#include "map"
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
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
#include <list>
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "lwip/err.h"
#include "lwip/sys.h"
#include "TcpTaskHandlerT.h"
// #ifdef __cplusplus
// extern "C" {
// #endif
#define NETWORK_CONTROLLER_BUFFER_MAX_LEN 128
#define NETWORK_CONTROLLER_TCP_PROXY_TIMEOUT_MS (1000*2)/10

namespace NetworkController{

extern const char *TAG;

struct wifi_st_conf_t
{
    int id;
    wifi_sta_config_t config;
};

struct wifi_ap_conf_t
{
    int id;
    wifi_ap_config_t config;
};






class NetworkController
{
private:
    // INTERFACES
    esp_netif_t *ap_netif;
	esp_netif_t *sta_netif;


    std::map<int, wifi_ap_conf_t> ApConfigurations;
    std::map<int, wifi_st_conf_t> StConfigurations;

    // TODO rename configurations to handlers
    std::map<int, TcpHandlerType_t> TcpConfigurationTypes; // ID of th, th
    std::map<int, void*> TcpConfigurations; // ID of th, th
    std::map<int, tcp_raw_task_handler_t*> TcpRawConfigurations; // port of raw th, raw th
    // std::map<int, int> TcpActivePortNumberToHandlerMap; // port, id of th NOT USED
    std::map<int, int> TcpActiveHandlerIdToPortNumberMap; // id of th, port

    int startingPortNum = 10000;
    int rollingPortNum = 0;

    int current_ap_config_id = -1;
    int current_st_config_id = -1;
    wifi_mode_t wifi_mode = WIFI_MODE_NULL;
    wifi_config_t current_configuration;
    // bool initialised = true;

    esp_err_t WifiRefresh();

    static void TcpRawTaskHandlerBase(void* pvParameters);

    esp_err_t TcpInstantiateRawHandlerForHandler(int handler_id); // returns raw handler id // done
    esp_err_t TcpInstantiateRawHandlerForHandler(int handler_id, int port); // done
    // esp_err_t TcpKillRawHandlerForPort(int port);

    int TcpAllocatePortNumber(); // done
    int TcpRegisterPortNumberAllocation(int port); // done
    void TcpDellocatePortNumber(int port); // done

public:
    Preferences_::PreferencesController* preferencesController = NULL;
    NetworkController(Preferences_::PreferencesController* preferencesController);
    
    int WifiScan(); // TODO
    esp_err_t WifiStart();
    esp_err_t WifiStop();


    esp_err_t ApStart(int config_id);
    esp_err_t ApStop();
    // esp_err_t ApConnect();
    // esp_err_t ApDisconnect();

    int ApAddConfiguration(wifi_ap_conf_t config);
    void ApRemoveConfiguration(int config_id);
    wifi_ap_conf_t ApGetConfiguration(int config_id);


    esp_err_t StStart(int config_id);
    esp_err_t StStop();
    int StAddConfiguration(wifi_st_conf_t config);
    void StRemoveConfiguration(int config_id);
    wifi_st_conf_t StGetConfiguration(int config_id);


    static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);


    esp_err_t TcpAddHandler(void* handler, TcpHandlerType_t type); // to map // done
    esp_err_t TcpRemoveHandler(int handler_id); // from map // done

    int  TcpBindHandlerToPort(int handler_id); // returns port number // done
    bool TcpBindHandlerToPort(int handler_id, int port); // returns success // done
    esp_err_t TcpKillHandler(int handler_id);  // done
    int TcpGetPortForHandlerId(int handler_id); // done

    esp_err_t TcpBindRawHandlerToPort(tcp_raw_task_handler_t* raw_task); 
    esp_err_t TcpKillRawHandlerForPort(int port); // done

    // create tcp_task_handler* -> call TcpAddHandler, call TcpBindHandlerToPort
    // call TcpKillHandler -> TcpRemoveHandler

    ~NetworkController();
};

// #ifdef __cplusplus
// }
// #endif
}