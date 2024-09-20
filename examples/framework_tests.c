void DevicePutInt32(int id, int value){
	return;
}

int DeviceGetInt32(int id){
	return -1;
}

int DeviceGetDHT11Value(int id){
	if(id == 11) return 7832465;
	else return -1;	
}

int DeviceGetColorSensorValue(int id){
	if(id == 8) return 1579050;
	else return -1;
}

int DeviceMoveServo(int id, int position){
	if(id == 1 || id == 2) return 1;
	else return -1;	
}

int DeviceGetSensorValue(int id){
	if(id == 5) return 21;
	else return -1;
}
