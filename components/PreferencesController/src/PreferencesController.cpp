#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_check.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "PreferencesController.h"
#include "lwip/sys.h"
#include "esp_wifi.h"
#include "lwip/sys.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/err.h"

namespace Preferences_{

    const char* TAG = "PreferencesController";

    PreferencesController::PreferencesController()
    {
        IP4_ADDR(&ipInfo.ip, 50,0,0,1);
        IP4_ADDR(&ipInfo.gw, 50,0,0,1);
	    IP4_ADDR(&ipInfo.netmask, 255,255,255,0);
    }

    void PreferencesController::ResetToDefault(){
        this->SetStr(Preferences_::ESP_WIFI_SSID, this->DEFAULT_ESP_WIFI_SSID);
        this->SetStr(Preferences_::ESP_WIFI_PASSWORD, this->DEFAULT_ESP_WIFI_PASS);
        this->SetStr(Preferences_::ESP_FIRMWARE_UPGRADE_URL, this->DEFAULT_FIRMWARE_UPGRADE_URL);
        this->SetI32(Preferences_::ESP_WIFI_MAX_RETRIES, this->DEFAULT_ESP_MAXIMUM_RETRY);
    }

    void PreferencesController::Erase(){
            nvs_flash_erase();
        };


    int PreferencesController::Boot(){
        if(this->didBoot) return this->didBoot;
        esp_err_t err ;
        ESP_LOGI(TAG, "Opening Non-Volatile Storage (NVS) handle... ");
        err = nvs_open(NVS_HANDLE, NVS_READWRITE, &this->_nvs_handle);        
        if (err != ESP_OK) {
            printf("Error (%d) opening NVS handle!\n", err);
        } else {
            printf("Done\n");
        }

        if(!this->ExistsStr(Preferences_::ESP_WIFI_SSID)) this->SetStr(Preferences_::ESP_WIFI_SSID, this->DEFAULT_ESP_WIFI_SSID);
        if(!this->ExistsStr(Preferences_::ESP_WIFI_PASSWORD)) this->SetStr(Preferences_::ESP_WIFI_PASSWORD, this->DEFAULT_ESP_WIFI_PASS);
        if(!this->ExistsStr(Preferences_::ESP_FIRMWARE_UPGRADE_URL)) this->SetStr(Preferences_::ESP_FIRMWARE_UPGRADE_URL, this->DEFAULT_FIRMWARE_UPGRADE_URL);
        if(!this->ExistsI32(Preferences_::ESP_WIFI_MAX_RETRIES)) this->SetI32(Preferences_::ESP_WIFI_MAX_RETRIES, this->DEFAULT_ESP_MAXIMUM_RETRY);

        this->didBoot = true;
        return this->didBoot;  
    }


    int PreferencesController::ExistsStr(NamedPreferencesCharPntr_t type){
        size_t required_size = 0;
        esp_err_t err;

        char type_c[2] = {(char)type, '\0'}; 

        err = nvs_get_str(this->_nvs_handle, type_c, NULL, &required_size);
        // sleep(1);

        switch (err) {
            case ESP_OK:
                ESP_LOGI(TAG, "Success exists value for str type : %d", (int)type);
                return required_size;
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGI(TAG, "Failure exists value for str type : %d", (int)type);
                return required_size;
                break;
            default :
                ESP_LOGI(TAG, "Error (%s) exists!\n", esp_err_to_name(err));
                return required_size;
        }
    }

    int PreferencesController::GetStr(NamedPreferencesCharPntr_t type, char* copy_to, int& length){
        size_t required_size = 0;
        esp_err_t err;

        required_size = this->ExistsStr(type);
        length = required_size;
        if(required_size == 0) return required_size;
        
        char type_c[2] = {(char)type, '\0'}; 
        copy_to = (char*)malloc(required_size * sizeof(char));
        nvs_get_str(this->_nvs_handle, type_c, copy_to, &required_size);

        return required_size;
    }

    bool PreferencesController::SetStr(NamedPreferencesCharPntr_t type, char* copy_from){
        char type_c[2] = {(char)type, '\0'}; 
        printf("\n5NVS : %d\n", this->_nvs_handle);
        printf("5VS : %02X\n", *type_c);

        esp_err_t err = nvs_set_str(this->_nvs_handle, type_c, copy_from);
        err = nvs_commit(this->_nvs_handle);

        if(err != ESP_OK){
            ESP_LOGI(TAG, "Set Failed for : %d", (int)type);
            return false;
        }
        else{
            printf("Set str with id : %s  ||||  %d\n", esp_err_to_name(err), (int)type);
        }
        
        
        if(err != ESP_OK){
            ESP_LOGI(TAG, "Commit Failed for : %d", (int)type);
            return false;
        }
        // sleep(1);
        return true;
    }


    bool PreferencesController::GetI32(NamedPreferencesI32_t type, int32_t& copy_to){
        esp_err_t err;
        if(!this->ExistsI32(type)) return false;
        
        char type_c[2] = {(char)type, '\0'}; 
        err = nvs_get_i32(this->_nvs_handle, type_c, &copy_to);

        switch (err) {
            case ESP_OK:
                ESP_LOGI(TAG, "Success getting value for str type : %d", (int)type);
                return true;
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGI(TAG, "Failure getting value for str type : %d", (int)type);
                return false;
                break;
            default :
                ESP_LOGI(TAG, "Error (%s) reading!\n", esp_err_to_name(err));
                return false;
        }
    }

    bool PreferencesController::SetI32(NamedPreferencesI32_t type, int32_t copy_from){
        char type_c[2] = {(char)type, '\0'}; 
        esp_err_t err = nvs_set_i32(this->_nvs_handle, type_c, copy_from);
        if(err != ESP_OK){
            ESP_LOGI(TAG, "Commit Failed for : %d", (int)type);
            return false;
        }    
        err = nvs_commit(this->_nvs_handle);
        if(err != ESP_OK){
            ESP_LOGI(TAG, "Commit Failed for : %d", (int)type);
            return false;
        }
        return true;
    }

    bool PreferencesController::ExistsI32(NamedPreferencesI32_t type){
        esp_err_t err;
        char type_c[2] = {(char)type, '\0'}; 
        int tmp_check = 1;
        err = nvs_get_i32(this->_nvs_handle, type_c, &tmp_check);
        // sleep(1);

        switch (err) {
            case ESP_OK:
                ESP_LOGI(TAG, "Success exists value for str type : %d", (int)type);
                return true;
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGI(TAG, "Failure exists value for str type : %d", (int)type);
                return false;
                break;
            default :
                ESP_LOGI(TAG, "Error (%s) exists!\n", esp_err_to_name(err));
                return false;
        }
    }

    PreferencesController::~PreferencesController()
    {
        nvs_close(this->_nvs_handle);
    }

}