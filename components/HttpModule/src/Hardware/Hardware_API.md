
# /hardware
```
{
    "idf_sdk_version":"",           // esp_get_idf_version()
    "mac_address":            int,  // esp_read_mac()
    "chip_info":{                   // esp_chip_info() 
        "model":    "",
        "features": int,
        "revision": int,
        "cores:     int
    },
    "memory":{
        "free_heap_size":         int,  // esp_get_free_heap_size()
        "free_heap_size_minimum": int,  // esp_get_minimum_free_heap_size()
    }

}
```