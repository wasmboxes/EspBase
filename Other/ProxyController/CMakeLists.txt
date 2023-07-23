idf_component_register(SRCS "src/ProxyController.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES mbedtls console spiffs nvs_flash PreferencesController)
