idf_component_register(SRCS "main.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi nvs_flash PreferencesController HwController NetworkController HttpParser Wasm3 HttpServer HttpModule BoxDevice
                    )
