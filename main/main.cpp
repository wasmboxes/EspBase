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

// #define WIFI_SSID "WrRtr"
// #define WIFI_PASS "odjedandoosam"


#define WIFI_SSID "b"
#define WIFI_PASS "odjedandoosam"


// #define ESP_WIFI_SSID "MESH"
// #define ESP_WIFI_PASS "12345678" // wifi password >= 8

// #include <iostream>
// #include "document.h"
// #include "writer.h"
// #include "stringbuffer.h"
// #include "rapidjson.h"


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


// #define CONFIG_LWIP_TCP_TMR_INTERVAL 10
// #define CONFIG_LWIP_MAX_RAW_PCBS 100

extern "C" {
    void app_main(void);
}

// extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
// extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

void app_main(void)
{

    ledc_timer_config_t ledc_timer;
    memset(&ledc_timer, 0, sizeof(ledc_timer_config_t));
    ledc_timer.duty_resolution = LEDC_TIMER_14_BIT; // resolution of PWM duty
    ledc_timer.freq_hz = 50;                      // frequency of PWM signal
    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;           // timer mode
    ledc_timer.timer_num = LEDC_TIMER_0;           // timer index
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t channel_config;
    memset(&channel_config, 0, sizeof(ledc_channel_config_t));
    channel_config.channel    = LEDC_CHANNEL_0;
    channel_config.duty       = 500;
    channel_config.gpio_num   = 8;
    channel_config.speed_mode = LEDC_LOW_SPEED_MODE;
    channel_config.timer_sel  = LEDC_TIMER_0;
    channel_config.intr_type = LEDC_INTR_DISABLE;
    ledc_channel_config(&channel_config);

    printf("\nFree heap : %d\n", esp_get_free_heap_size());

    BoxDevice::BoxDevices* devices = new BoxDevice::BoxDevices();

    BoxDevice::BoxDevice_Servo* servo = new BoxDevice::BoxDevice_Servo(BoxDevice::BOX_DEVICE_TYPE::SERVO, 1, 8);
    
    devices->Add(servo);

    printf("\n1111111111111Free heap : %d\n", esp_get_free_heap_size());


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

    WasmController::WasmSandbox* box = new WasmController::WasmSandbox();
    unsigned int ok = c_wm->SandboxCreate(box,fib32_wasm, fib32_wasm_len, "Fib32");

    WasmController::WasmSandbox* box_2 = new WasmController::WasmSandbox();
    ok = c_wm->SandboxCreate(box_2, multiple2_wasm, multiple2_wasm_len, "Multiple");

    WasmController::WasmSandbox* box_3 = new WasmController::WasmSandbox();
    ok = c_wm->SandboxCreate(box_3, multiple3_wasm, multiple3_wasm_len, "AddTwo");

    WasmController::WasmSandbox* box_4 = new WasmController::WasmSandbox();
    ok = c_wm->SandboxCreate(box_4, fib_random, fib_random_len, "FibRandom");

    WasmController::WasmSandbox* box_5 = new WasmController::WasmSandbox();
    ok = c_wm->SandboxCreate(box_5, moveServo_wasm, moveServo_wasm_len, "MoveServo");

    printf("\nBox 1 id : %d  Box 2 id : %d\n", box->id, box_2->id);
    // WasmController::WASM_STATUS status = c_wm->SandboxHasFunction(box_2->id, "fib");
    // if(status == WasmController::ERROR){
    //     printf("WASM ERROR FUNCTION  \n");
    // }if(status == WasmController::NOT_FOUND){
    //     printf("WASM FUNCTION NOT FOUND \n");
    // }if(status == WasmController::OK){
    //     printf("WASM FUNCTION FOUND \n");
        // IM3Function function;
        // WasmController::WASM_STATUS status = c_wm->SandboxCallFunctionV(box_2->id, "fib", function, 5);

        // if(status == WasmController::ERROR){
        //     printf("WASM ERROR FUNCTION CALL  \n");
        // }if(status == WasmController::NOT_FOUND){
        //     printf("WASM FUNCTION CALL NOT FOUND \n");
        // }if(status == WasmController::OK){
        //     printf("WASM FUNCTION CALLED \n");
        //     unsigned value = 0;
        //     // result = m3_GetResultsV (result, &value);   
        //     c_wm->SandboxGetResults(function, &value);
        //     printf("Result: %u\n", value); 
        // }   
        // status = c_wm->SandboxCallFunctionV(box_2->id, "fac", function, 3);

        // if(status == WasmController::ERROR){
        //     printf("WASM ERROR FUNCTION CALL  \n");
        // }if(status == WasmController::NOT_FOUND){
        //     printf("WASM FUNCTION CALL NOT FOUND \n");
        // }if(status == WasmController::OK){
        //     printf("WASM FUNCTION CALLED \n");
        //     unsigned value = 0;
        //     // result = m3_GetResultsV (result, &value);   
        //     c_wm->SandboxGetResults(function, &value);
        //     printf("Result: %u\n", value); 
        // }   
    // }

    // printf("WASM LOAD RES : %d\n", box->error);


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


    // wifi_ap_config_t wifi_config_ap = {};
    // strcpy((char*)wifi_config_ap.ssid, (const char*)ESP_WIFI_SSID);
    // strcpy((char*)wifi_config_ap.password, (const char*)ESP_WIFI_PASS);
    // wifi_config_ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
	// wifi_config_ap.max_connection = 8;

    // NetworkController::wifi_ap_conf_t b;
    // b.config = wifi_config_ap;
    // b.id = 1;
    // c_nw->ApAddConfiguration(b);
    // c_nw->ApStart(1);

    //////////////////////////////////////////////////////////////////////////////////////////

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("STARTING SERVERS\n");

    //////////////////////////////////////////////////////////////////////////////////////////
    // NetworkController::tcp_raw_task_handler_t* raw_task_handler = NetworkController::tcp_raw_task_handler_instantiate();
    // NetworkController::tcp_echo_task_handler_t* task_handler = NetworkController::tcp_echo_task_handler_instantiate();
    // *task_handler->id = 20;
    // task_handler->service_name = "TEST\0";
    // raw_task_handler->handler_type = NetworkController::Echo;
    // raw_task_handler->handler = task_handler;
    // raw_task_handler->port = 10005;
    // c_nw->TcpBindRawHandlerToPort(raw_task_handler);
    // //////////////////////////////////////////////////////////////////////////////////////////
    // NetworkController::tcp_raw_task_handler_t* raw_task_handler_ = NetworkController::tcp_raw_task_handler_instantiate();
    // NetworkController::tcp_transceiver_task_handler_t* task_handler_ = NetworkController::tcp_transceiver_task_handler_instantiate();
    // HttpServer::SimpleHttpServerBase shs;

    // *task_handler_->id = 21;
    // task_handler_->service_name = "TEST\0";
    // task_handler_->SocketHandler = (NetworkController::TcpTranscieverBase*)&shs;
    // raw_task_handler_->handler_type = NetworkController::Transceiver;
    // raw_task_handler_->handler = task_handler_;
    // raw_task_handler_->port = 10006;
    // c_nw->TcpBindRawHandlerToPort(raw_task_handler_);
    // //////////////////////////////////////////////////////////////////////////////////////////
    // NetworkController::tcp_raw_task_handler_t* raw_task_handler_proxy = NetworkController::tcp_raw_task_handler_instantiate();
    // NetworkController::tcp_proxy_task_handler_t* task_handler_proxy = NetworkController::tcp_proxy_task_handler_instantiate();
    // *task_handler_proxy->id = 22;
    // task_handler_proxy->service_name = "Proxy\0";
    // task_handler_proxy->dest_addr.sin_addr.s_addr = inet_addr("10.0.0.100");
    // task_handler_proxy->dest_addr.sin_family = AF_INET;
    // task_handler_proxy->dest_addr.sin_port = htons(4000);
    // raw_task_handler_proxy->handler = task_handler_proxy;
    // raw_task_handler_proxy->handler_type = NetworkController::Proxy;
    // raw_task_handler_proxy->port = 10007;
    // c_nw->TcpBindRawHandlerToPort(raw_task_handler_proxy);
    //////////////////////////////////////////////////////////////////////////////////////////

    //vTaskDelay(10000 / portTICK_PERIOD_MS);

    //////////////////////////////////////////////////////////////////////////////////////////
    // NetworkController::tcp_echo_task_handler_t* echo_task_handler = NetworkController::tcp_echo_task_handler_instantiate();
    // echo_task_handler->id = (int*)malloc(sizeof(int));
    // *echo_task_handler->id = 25;
    // echo_task_handler->service_name = "TEST\0";
    // c_nw->TcpAddHandler(echo_task_handler, NetworkController::Echo);
    // c_nw->TcpBindHandlerToPort(25);
    //////////////////////////////////////////////////////////////////////////////////////////
    // ESP_LOGI("MAIN", "Starting Raw TH");
    // NetworkController::tcp_raw_transceiver_task_handler_t* tcp_raw_task_handler = NetworkController::tcp_raw_transceiver_task_handler_instantiate();
    // HttpServer::HttpServerBase hsb;
    // tcp_raw_task_handler->id = (int*)malloc(sizeof(int));
    // *tcp_raw_task_handler->id = 50;
    // tcp_raw_task_handler->service_name = "Raw Tcp\0";
    // tcp_raw_task_handler->SocketHandler = (NetworkController::TcpRawTranscieverBase*)&hsb;
    // c_nw->TcpAddHandler(tcp_raw_task_handler, NetworkController::RawTransceiver);
    // c_nw->TcpBindHandlerToPort(50);
    //////////////////////////////////////////////////////////////////////////////////////////
    ESP_LOGI("MAIN", "Starting HTTP TH");
    NetworkController::tcp_raw_transceiver_task_handler_t* tcp_raw_task_handler_http_module = NetworkController::tcp_raw_transceiver_task_handler_instantiate();
    // HttpServer::HttpServerBase hsb;
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
        
    //////////////////////////////////////////////////////////////////////////////////////////

    // 1. Parse a JSON string into DOM.
    // const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    // rapidjson::Document d;
    // d.Parse(json);
    // // 2. Modify it by DOM.
    // rapidjson::Value& s = d["stars"];
    // s.SetInt(s.GetInt() + 1);

    // // 3. Stringify the DOM
    // rapidjson::StringBuffer buffer;
    // rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    // d.Accept(writer);

    // // Output {"project":"rapidjson","stars":11}
    // std::cout << buffer.GetString() << std::endl;
    // std::string sfsd(buffer.GetString(), buffer.GetSize());
    // std::cout<<sfsd<<std::endl;

    //////////////////////////////////////////////////////////////////////////////////////////


    // printf("WASM RES : %d\n", run_wasm());





    // //////////////////////////////////////////////////////////////////////////////////////////


    // vTaskDelay(1000 / portTICK_PERIOD_MS);

    // printf("WASM RES : %d\n", run_wasm_print()); 

    
    // vTaskDelay(10000 / portTICK_PERIOD_MS);

    // printf("\nFree heap : %d\n", esp_get_free_heap_size());

    // printf("WASM RES : %d\n", run_wasm_getString());

    // printf("\nFree heap : %d\n", esp_get_free_heap_size());

    // ledc_timer_config_t ledc_timer;
    // memset(&ledc_timer, 0, sizeof(ledc_timer_config_t));
    // ledc_timer.duty_resolution = LEDC_TIMER_13_BIT; // resolution of PWM duty
    // ledc_timer.freq_hz = 50;                      // frequency of PWM signal
    // ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;           // timer mode
    // ledc_timer.timer_num = LEDC_TIMER_0;           // timer index
    
    // ledc_timer_config(&ledc_timer);

    // ledc_channel_config_t channel_config;
    // memset(&channel_config, 0, sizeof(ledc_channel_config_t));
    // channel_config.channel    = LEDC_CHANNEL_0;
    // channel_config.duty       = 2000;
    // channel_config.gpio_num   = 8;
    // channel_config.speed_mode = LEDC_LOW_SPEED_MODE;
    // channel_config.timer_sel  = LEDC_TIMER_0;
    // channel_config.intr_type = LEDC_INTR_DISABLE;



    // servo->Move(90);
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    // servo->Move(170);
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    // servo->Move(10);
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    // servo->Move(20);
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    // servo->Move(100);
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    // servo->Move(150);
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    // servo->Move(0);
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    // servo->Move(180);
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    // servo->Move(0);
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    // servo->Move(180);
    // vTaskDelay(2000 / portTICK_PERIOD_MS);

    // c_nw->TcpKillHandler(25); //!WARNING
    int DONT_RESTART = 1;
    while(1) {
        DONT_RESTART ++;
        // ESP_LOGI("MAIN", "While loop");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}