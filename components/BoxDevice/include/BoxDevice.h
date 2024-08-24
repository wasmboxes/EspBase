#pragma once
#include <stdio.h>
#include "esp_err.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "BoxDevice.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "PreferencesController.h"
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
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/ledc.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include <map>

namespace BoxDevice{

enum BOX_DEVICE_TYPE{
    NONE            = 0,
    COLOR_SENSOR    = 1,
    LIGHT_SENSOR    = 2,
    SERVO           = 3
};  

class BoxDevice{
private:
    BOX_DEVICE_TYPE type = BOX_DEVICE_TYPE::NONE;
    int id = 0;

public:
    BoxDevice(BOX_DEVICE_TYPE type, int id);

    BOX_DEVICE_TYPE GetType();
    int GetId();

    ~BoxDevice();
};

class BoxDevices{
private:
    std::map<int, BoxDevice*> devices;

public:
    BoxDevices();

    void Add(BoxDevice* device);
    void Remove(int id);

    void PutUInt32(uint32_t id, uint32_t value);
    uint32_t GetUInt32(uint32_t id);

    ~BoxDevices();
};


class BoxDevice_Servo : public BoxDevice{
private:
    int pin = 0;
    
    ledc_channel_config_t   ledc_conf;
    ledc_timer_config_t     timer_conf;
    int minValue            = 500;  
	int maxValue            = 2500;
    int delta               = 2000;
    int duty                = 500;


public:
    BoxDevice_Servo(BOX_DEVICE_TYPE type, int id, int pin);

    void Move(int position);
    static void Init(void *task);

    ~BoxDevice_Servo();
};



} // namespace BoxDevice 
