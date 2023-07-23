#pragma once

#ifndef NPREF
#define PREF
#include <stdio.h>
#include "esp_err.h"
#include <string>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/err.h"

namespace Preferences_{

    #define NVS_HANDLE "storage"

    extern const char *TAG;

    typedef enum NamedPreferencesI32_t {
        ESP_WIFI_MAX_RETRIES = 13,
    } NamedPreferencesI32;

    typedef enum NamedPreferencesCharPntr_t {
        ESP_WIFI_PASSWORD = 10,
        ESP_WIFI_SSID = 11,
        ESP_FIRMWARE_UPGRADE_URL = 14,
    } NamedPreferencesCharPntr;

    // typedef enum NamedPreferencesI32_t;
    // typedef enum NamedPreferencesCharPntr_t;

    class PreferencesController
    {
    
    private:
        int     DEFAULT_ESP_HW_UID                  = 1234;
        char    DEFAULT_ESP_WIFI_SSID[6]            = "node\0";
        char    DEFAULT_ESP_WIFI_PASS[16]           = "odjedandodosam\0";
        int     DEFAULT_ESP_MAXIMUM_RETRY           = 10;
        char    DEFAULT_FIRMWARE_UPGRADE_URL[13]    = "master.mesh\0";
        bool didBoot = false;
        nvs_handle_t _nvs_handle;
    public:
        esp_netif_ip_info_t ipInfo;
    
        PreferencesController();

        int Boot();
        
        bool ExistsI32(NamedPreferencesI32_t type);
        bool GetI32(NamedPreferencesI32_t type, int32_t& copy_to);
        bool SetI32(NamedPreferencesI32_t type, int32_t copy_from);

        int ExistsStr(NamedPreferencesCharPntr_t type); // return length
        int GetStr(NamedPreferencesCharPntr_t type, char* copy_to, int& length);
        bool SetStr(NamedPreferencesCharPntr_t type, char* copy_to);

        void Erase();
        void ResetToDefault();

        ~PreferencesController();
    };
}

#endif // NPREF
