#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "BoxDevice.h"
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
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/ledc.h"
#include "sdkconfig.h"
#include "esp_log.h"

namespace BoxDevice
{

const char* TAG = "BoxDevice";

BOX_DEVICE_TYPE BoxDevice::GetType(){
    return this->type;
}

int BoxDevice::GetId(){
    return this->id;
}

BoxDevice::BoxDevice(BOX_DEVICE_TYPE type, int id)
{
    this->type = type;
    this->id = id;
}

BoxDevice::~BoxDevice()
{
}


BoxDevices::BoxDevices(){
	
}

void BoxDevices::Add(BoxDevice* device){
	this->devices[device->GetId()] = device;
}

void BoxDevices::Remove(int id){
	this->devices.erase(id);
}

void BoxDevices::PutUInt32(uint32_t id, uint32_t value){
	ESP_LOGI(TAG,"AAA PutUInt32");
	printf("\n1111111111111 : %d\n", this->devices);

	if(this->devices.find(1) != this->devices.end()){
		BOX_DEVICE_TYPE type = this->devices.at(1)->GetType();
		ESP_LOGI(TAG,"BBB PutUInt32");
		switch (type)
		{
			case BOX_DEVICE_TYPE::SERVO :
				((BoxDevice_Servo*)this->devices[1])->Move(value);
			case BOX_DEVICE_TYPE::COLOR_SENSOR :
			case BOX_DEVICE_TYPE::LIGHT_SENSOR :
			case BOX_DEVICE_TYPE::NONE :
			default:
				break;
		}
	}
}

uint32_t BoxDevices::GetUInt32(uint32_t id){
	ESP_LOGI(TAG,"GetUInt32");
	return 5;
}

BoxDevices::~BoxDevices(){
	
}


BoxDevice_Servo::BoxDevice_Servo(BOX_DEVICE_TYPE type, int id, int pin) : BoxDevice(type, id)
{
    this->pin = pin;
	// xTaskCreate(&this->Init,"InitServo_task",2048,(void*)this,5,NULL);
	// vTaskDelay(2000/portTICK_PERIOD_MS);
}

void BoxDevice_Servo::Init(void *device){
	// BoxDevice_Servo* servo = (BoxDevice_Servo*) device;

	// servo->timer_conf.bit_num    = LEDC_TIMER_14_BIT;
	// servo->timer_conf.freq_hz    = 50;
	// servo->timer_conf.speed_mode = LEDC_LOW_SPEED_MODE;
	// servo->timer_conf.timer_num  = LEDC_TIMER_1;
	// servo->timer_conf.clk_cfg    = LEDC_AUTO_CLK;
	// ledc_timer_config(&servo->timer_conf);

	// servo->ledc_conf.channel    = LEDC_CHANNEL_0;
	// servo->ledc_conf.duty       = servo->duty;
	// servo->ledc_conf.gpio_num   = servo->pin;
	// servo->ledc_conf.intr_type  = LEDC_INTR_DISABLE;
	// servo->ledc_conf.speed_mode = LEDC_LOW_SPEED_MODE;
	// servo->ledc_conf.timer_sel  = LEDC_TIMER_1;
	// ledc_channel_config(&servo->ledc_conf);


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


	// int minValue        = 500;  // micro seconds (uS)
	// int maxValue        = 2500; // micro seconds (uS)
    // int delta = maxValue - minValue;;
    // int duty;

	// // ledc_timer_config_t timer_conf = {0};
	// // ledc_channel_config_t ledc_conf = {0};

    // ledc_timer_config_t timer_conf;
    // memset(&timer_conf, 0, sizeof(ledc_timer_config_t));

    // duty = minValue ; 
	// // ledc_timer_config_t timer_conf;
	// timer_conf.bit_num    = LEDC_TIMER_14_BIT;
	// timer_conf.freq_hz    = 50;
	// timer_conf.speed_mode = LEDC_LOW_SPEED_MODE;
	// timer_conf.timer_num  = LEDC_TIMER_0;
	// timer_conf.clk_cfg    = LEDC_AUTO_CLK;
	// ledc_timer_config(&timer_conf);

	// ledc_channel_config_t ledc_conf;
	// ledc_conf.channel    = LEDC_CHANNEL_1;
	// ledc_conf.duty       = duty;
	// ledc_conf.gpio_num   = 8;
	// ledc_conf.intr_type  = LEDC_INTR_DISABLE;
	// ledc_conf.speed_mode = LEDC_LOW_SPEED_MODE;
	// ledc_conf.timer_sel  = LEDC_TIMER_1;
	// ledc_channel_config(&ledc_conf);

}

void BoxDevice_Servo::Move(int position){
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, this->duty+ (this->delta*(position/180.0)) );
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0); 
}

BoxDevice_Servo::~BoxDevice_Servo()
{
}

}