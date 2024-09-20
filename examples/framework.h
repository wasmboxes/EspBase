extern "C"{

        extern void DevicePutInt32(int id, int value);
        extern int DeviceGetInt32(int id);
        extern int DeviceGetDHT11Value(int id);
        extern int DeviceGetColorSensorValue(int id);
		extern int DeviceMoveServo(int id, int position);
		extern int DeviceGetSensorValue(int id);

        int BOX_DEVICE_TYPE_NONE = 0;
        int BOX_DEVICE_TYPE_COLOR_SENSOR = 1;
        int BOX_DEVICE_TYPE_LIGHT_SENSOR = 2;
        int BOX_DEVICE_TYPE_SERVO = 3;
        int BOX_DEVICE_TYPE_DHT11 = 4;

}
