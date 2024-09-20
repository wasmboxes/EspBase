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
#include "esp_vfs.h"
#include "esp_vfs_common.h"

#include "PreferencesController.h"
#include "HwController.h"
#include "NetworkController.h"
#include "HttpParser.h"

#include "lwip/ip4.h"
#include "lwip/udp.h"
#include "lwip/inet_chksum.h"
#include "lwip/stats.h"
#include "lwip/prot/ip.h"
#include "lwip/prot/ip4.h"
#include "lwip/lwip_napt.h"
#include "lwip/priv/tcp_priv.h"

#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

char *TAG_WIFI_STATION = "wifi station";
char *TAG = "MAIN";
char *TAG_OTA_XTASK = "ota";


#define WIFI_SSID "b"
#define WIFI_PASS "odjedandoosam"


#include "./wasm_test.h"
#include <list>

#include "HttpServer.h"

#include "HttpModule.h"
#include "HttpModuleController.h"
#include "Hardware/HardwareModule.h"
#include "Wasm/WasmModule.h"
#include "BoxDevice.h"

#include "ArduinoJson.h"
#include "WasmController.h"
#include "wasm_programs.h"



extern "C" {
    void app_main(void);
}

// extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
// extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

void app_main(void)
{

    // ledc_timer_config_t ledc_timer;
    // memset(&ledc_timer, 0, sizeof(ledc_timer_config_t));
    // ledc_timer.duty_resolution = LEDC_TIMER_14_BIT; 
    // ledc_timer.freq_hz = 50;                     
    // ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;           
    // ledc_timer.timer_num = LEDC_TIMER_0;          
    // ledc_timer_config(&ledc_timer);

    // ledc_channel_config_t channel_config;
    // memset(&channel_config, 0, sizeof(ledc_channel_config_t));
    // channel_config.channel    = LEDC_CHANNEL_0;
    // channel_config.duty       = 500;
    // channel_config.gpio_num   = 8;
    // channel_config.speed_mode = LEDC_LOW_SPEED_MODE;
    // channel_config.timer_sel  = LEDC_TIMER_0;
    // channel_config.intr_type = LEDC_INTR_DISABLE;
    // ledc_channel_config(&channel_config);

    printf("\nFree heap : %d\n", esp_get_free_heap_size());

    BoxDevice::BoxDevices* devices = new BoxDevice::BoxDevices();

    BoxDevice::BoxDevice_Servo* servo = new BoxDevice::BoxDevice_Servo(BoxDevice::BOX_DEVICE_TYPE::SERVO, 1, 8);
    
    devices->Add(servo);


    Preferences_::PreferencesController* c_pf = new Preferences_::PreferencesController(); 
    HwController* c_hw = new HwController(c_pf);
    c_hw->Boot();

    NetworkController::tcp_raw_task_handler_t* ad = NetworkController::tcp_raw_task_handler_instantiate();


    NetworkController::tcp_raw_active_connection* handle = new NetworkController::tcp_raw_active_connection();
    handle->xHandle = NULL;
    ad->active_connections->push_back(handle);

    NetworkController::NetworkController* c_nw = new NetworkController::NetworkController(c_pf); 

    /////////////////////////////////////////

    WasmController::WasmController* c_wm = new WasmController::WasmController();
    c_wm->LinkDevices(devices);

    // WasmController::WasmSandbox* box = new WasmController::WasmSandbox();
    // unsigned int ok = c_wm->SandboxCreate(box,fib32_wasm, fib32_wasm_len, "Fib32");

    // WasmController::WasmSandbox* box_2 = new WasmController::WasmSandbox();
    // ok = c_wm->SandboxCreate(box_2, multiple2_wasm, multiple2_wasm_len, "Multiple");

    // WasmController::WasmSandbox* box_3 = new WasmController::WasmSandbox();
    // ok = c_wm->SandboxCreate(box_3, multiple3_wasm, multiple3_wasm_len, "AddTwo");

    // WasmController::WasmSandbox* box_4 = new WasmController::WasmSandbox();
    // ok = c_wm->SandboxCreate(box_4, fib_random, fib_random_len, "FibRandom");

    // WasmController::WasmSandbox* box_5 = new WasmController::WasmSandbox();
    // ok = c_wm->SandboxCreate(box_5, moveServo_wasm, moveServo_wasm_len, "MoveServo");

    WasmController::WasmSandbox* box_6 = new WasmController::WasmSandbox();
    unsigned int ok = c_wm->SandboxCreate(box_6, hash_wasm, hash_wasm_len, "HashOffloading");

    // printf("\nBox 1 id : %d  Box 2 id : %d\n", box->id, box_2->id);

    printf("STARTING STA\n");
    vTaskDelay(500 / portTICK_PERIOD_MS);

    wifi_sta_config_t wifi_config_sta = {};
    strcpy((char*)wifi_config_sta.ssid, (const char*)WIFI_SSID);
    strcpy((char*)wifi_config_sta.password, (const char*)WIFI_PASS);

    NetworkController::wifi_st_conf_t c;
    c.config = wifi_config_sta;
    c.id = 0;
    c_nw->StAddConfiguration(c);
    c_nw->StStart(0);


    //////////////////////////////////////////////////////////////////////////////////////////

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("STARTING SERVERS\n");

    //////////////////////////////////////////////////////////////////////////////////////////

    ESP_LOGI("MAIN", "Starting HTTP TH");
    NetworkController::tcp_raw_transceiver_task_handler_t* tcp_raw_task_handler_http_module = NetworkController::tcp_raw_transceiver_task_handler_instantiate();
    HttpModule::HttpModuleController http_controller;

    HardwareModule::HardwareModule http_controller_hw;
    http_controller_hw.HttpModuleBase::endpoint = "hardware";
    http_controller_hw.HttpModuleBase::id = 1;
    http_controller.AddModule((HttpModule::HttpModuleBase*)&http_controller_hw);

    WasmModule::WasmModule http_controller_wasm;
    http_controller_wasm.HttpModuleBase::endpoint = "wasm";
    http_controller_wasm.wasmController = c_wm;
    http_controller_wasm.HttpModuleBase::id = 2;
    http_controller.AddModule((HttpModule::HttpModuleBase*)&http_controller_wasm);

    tcp_raw_task_handler_http_module->id = (int*)malloc(sizeof(int));
    *tcp_raw_task_handler_http_module->id = 60;
    tcp_raw_task_handler_http_module->service_name = "Http Module\0";
    tcp_raw_task_handler_http_module->SocketHandler = (NetworkController::TcpRawTranscieverBase*)&http_controller;
    c_nw->TcpAddHandler(tcp_raw_task_handler_http_module, NetworkController::RawTransceiver);
    c_nw->TcpBindHandlerToPort(60);
    //////////////////////////////////////////////////////////////////////////////////////////

    ESP_LOGI("MAIN", "While loop starting");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
        
    int DONT_RESTART = 1;
    while(1) {
        DONT_RESTART ++;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}