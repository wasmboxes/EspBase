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
}

void BoxDevice_Servo::Init(void *device){

}

void BoxDevice_Servo::Move(int position){
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, this->duty+ (this->delta*(position/180.0)) );
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0); 
}

BoxDevice_Servo::~BoxDevice_Servo()
{
}

}