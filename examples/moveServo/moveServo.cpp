#include "framework.h"

void CloseServoIfTempOver40D(int servoId){
	if((int)(DeviceGetDHT11Value(4) / 100) > 40){
		DeviceMoveServo(servoId, 180);
	}
	else{
		return;
	}
}

int OpenServoAndReturnHumidity(int servoId){
	int servoMotorResponseCode = DeviceMoveServo(servoId, 90);
	if(servoMotorResponseCode < 0) return servoMotorResponseCode + 50000;
	else return DeviceGetDHT11Value(4) % 10;
}