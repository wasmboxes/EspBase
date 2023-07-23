#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "HwController.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "PreferencesController.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "PreferencesController.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"


void HwController::Boot(){
    printf("Boot Start\n");

    // esp_bt_controller_mem_release(ESP_BT_MODE_BTDM);

    // NVS FLASH INIT
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
    
    // ESP_ERROR_CHECK(esp_netif_init());
    // ESP_ERROR_CHECK(esp_event_loop_create_default());

    // esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
	// assert(ap_netif);
	// esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
	// assert(sta_netif);

    // Preferences INIT
    if(this->preferencesController != NULL) this->preferencesController->Boot();
    printf("Boot End\n");
}


HwController::HwController(Preferences_::PreferencesController* preferencesController)
{
    this->preferencesController = preferencesController;
}

HwController::~HwController()
{
}
