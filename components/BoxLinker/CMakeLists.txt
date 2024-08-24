idf_component_register(SRCS "src/BoxLinker.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES mbedtls console spiffs nvs_flash PreferencesController)
