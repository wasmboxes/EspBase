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

namespace NetworkController{

// const char* TAG = "NetworkController\0";
const char* TAG = "NetworkController";

NetworkController::NetworkController(Preferences_::PreferencesController* preferencesController)
{
    this->preferencesController = preferencesController;  

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    this->ap_netif = esp_netif_create_default_wifi_ap();
	assert(ap_netif);
	this->sta_netif = esp_netif_create_default_wifi_sta();
	assert(sta_netif);


    esp_netif_ip_info_t ipInfo;
    IP4_ADDR(&ipInfo.ip, 50,0,0,1);
    IP4_ADDR(&ipInfo.gw, 50,0,0,1);
    IP4_ADDR(&ipInfo.netmask, 255,255,255,0);

	esp_netif_dhcps_stop(ap_netif);
	esp_netif_set_ip_info(ap_netif, &ipInfo);
	esp_netif_dhcps_start(ap_netif);

	//esp_netif_dhcps_stop(ap_netif);
	// esp_netif_set_ip_info(ap_netif, &preferencesController->ipInfo);


    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();    
    esp_err_t err = esp_wifi_init(&cfg);
    

    
    switch (err)
    {
        case ESP_OK:
            ESP_LOGI(TAG,"Wifi init ok ");
            break;
        case ESP_ERR_NO_MEM:
            ESP_LOGI(TAG,"Wifi init error : %s ",esp_err_to_name(err));
        break;
        default:
            ESP_LOGI(TAG,"Wifi init error : %s ",esp_err_to_name(err));
        break;
    }

    // ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
    //                                                     ESP_EVENT_ANY_ID,
    //                                                     &wifi_event_handler,
    //                                                     NULL,
    //                                                     NULL));
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL) );
	ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL) );

    this->WifiStart();
}

esp_err_t NetworkController::WifiStart(){
    // ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
	// ESP_ERROR_CHECK( esp_wifi_set_mode(this->wifi_mode) );
    return esp_wifi_start();
}

esp_err_t NetworkController::WifiStop(){
    return esp_wifi_stop();
}

NetworkController::~NetworkController()
{

}

esp_err_t NetworkController::WifiRefresh(){
   // this->WifiStop();
    ESP_LOGI(TAG, "Wifi refersh wifi stopped.");
    wifi_mode_t mode = WIFI_MODE_NULL;// = WIFI_MODE_STA;

    ESP_LOGI(TAG, "Wifi WifiRefresh config id | sta : %d | ap : %d |", this->current_st_config_id, this->current_ap_config_id);

    if(this->current_st_config_id != -1){
        mode = WIFI_MODE_STA;
        ESP_LOGI(TAG,"Wifi refersh mode sta ");
        this->current_configuration.sta = this->StConfigurations[this->current_st_config_id].config;
        ESP_LOGI(TAG,"Wifi refersh config sta id : %d ", this->current_st_config_id);
    }
    if(this->current_ap_config_id != -1){
        if(mode == WIFI_MODE_STA) mode = WIFI_MODE_APSTA;
        else{
            mode = WIFI_MODE_AP;
            ESP_LOGI(TAG,"Wifi refersh mode ap ");
        }
        this->current_configuration.ap = this->ApConfigurations[this->current_ap_config_id].config;
        ESP_LOGI(TAG,"Wifi refersh config ap id : %d ", this->current_ap_config_id);
        ESP_LOGI(TAG,"Wifi refersh mode apsta ");
    }
    else if(mode == WIFI_MODE_NULL){ // if its null, exit and dont start wifi
        this->wifi_mode = WIFI_MODE_NULL;
        ESP_LOGI(TAG,"Wifi refersh mode null, exitiing without starting wifi ");
        return ESP_ERR_WIFI_STATE; // if not AP or STA or APSTA
    }

    if (strlen((char*)this->current_configuration.ap.password) == 0) {
        this->current_configuration.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(mode));

    ESP_LOGI(TAG,"Wifi refersh current mode int : %d", mode);

    if(mode == WIFI_MODE_AP){
        ESP_LOGI(TAG,"Wifi refersh setting ap config ");
        wifi_config_t cnfg = {}; // TODO CHECK IF NEEDED
        cnfg.ap = this->current_configuration.ap;
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &cnfg));
        this->WifiStart();
    }
    else if(mode == WIFI_MODE_STA) 
    {
        wifi_config_t cnfg = {}; // TODO CHECK IF NEEDED
        cnfg.sta = this->current_configuration.sta;
        ESP_LOGI(TAG,"Wifi refersh setting sta config ");
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cnfg));
        this->WifiStart();
        ESP_ERROR_CHECK( esp_wifi_connect());
    }
    else if(mode == WIFI_MODE_APSTA) 
    {
        wifi_config_t cnfg = {}; // TODO CHECK IF NEEDED
        cnfg.sta = this->current_configuration.sta;
        cnfg.ap = this->current_configuration.ap; 
        ESP_LOGI(TAG,"Wifi refersh setting apsta config ");
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cnfg));
        this->WifiStart();
        ESP_ERROR_CHECK( esp_wifi_connect()); //!WARNING OVDJE PUKNE sa ESP_ERR_WIFI_CONN
    }
    
    // this->WifiStart();
    // if(mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA) ESP_ERROR_CHECK( esp_wifi_connect());
    ESP_LOGI(TAG,"Wifi refersh finished");
    return ESP_OK;
}

esp_err_t NetworkController::ApStart(int config_id){
    if(this->ApConfigurations.find(config_id) != this->ApConfigurations.end()){
        this->current_ap_config_id = config_id;
    }
    else ESP_LOGI(TAG, "Wifi ApStart Error config not found.");

    this->WifiRefresh();
    ESP_LOGI(TAG, "Wifi ApStart finished. SSID:%s password:%s channel:%d",
              this->current_configuration.ap.ssid, this->current_configuration.ap.password, this->current_configuration.ap.channel);
    return ESP_OK;
}


esp_err_t NetworkController::StStart(int config_id){
    if(this->StConfigurations.find(config_id) != this->StConfigurations.end()){
        this->current_st_config_id = config_id;
        ESP_LOGI(TAG, "Wifi StStart found config with id %d.", config_id);
    }
    else ESP_LOGI(TAG, "Wifi StStart Error config not found.");
    this->WifiRefresh();
    ESP_LOGI(TAG, "Wifi StStart finished.");
    return ESP_OK;
}

esp_err_t NetworkController::StStop(){
    this->current_st_config_id = -1;
    this->WifiRefresh();
    ESP_LOGI(TAG, "Wifi StStop finished.");
    return ESP_OK;
}


esp_err_t NetworkController::ApStop(){
    this->current_ap_config_id = -1;
    this->WifiRefresh();
    ESP_LOGI(TAG, "Wifi ApStop finished.");
    return ESP_OK;
}

int NetworkController::ApAddConfiguration(wifi_ap_conf_t config){
    this->ApConfigurations[config.id] = config;
    return config.id; // TODO return config ID
}

void NetworkController::ApRemoveConfiguration(int config_id){
    this->ApConfigurations.erase(config_id);  
}

wifi_ap_conf_t NetworkController::ApGetConfiguration(int config_id){
    return this->ApConfigurations[config_id];
}

int NetworkController::StAddConfiguration(wifi_st_conf_t config){
    this->StConfigurations[config.id] = config;
    ESP_LOGI(TAG,"Wifi st add id : %d ", this->StConfigurations[config.id].id);
    return config.id; // TODO return config ID
}

void NetworkController::StRemoveConfiguration(int config_id){
    this->ApConfigurations.erase(config_id);  
}

wifi_st_conf_t NetworkController::StGetConfiguration(int config_id){
    return this->StConfigurations[config_id];
}

void NetworkController::wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                MAC2STR(event->mac), event->aid);
    }
}
void NetworkController::event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
		ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
		// esp_wifi_connect();
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
	}
}



}