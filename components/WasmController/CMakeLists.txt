idf_component_register(SRCS "src/WasmController.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES mbedtls console spiffs nvs_flash PreferencesController Wasm3 BoxDevice)
