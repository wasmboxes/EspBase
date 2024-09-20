idf_component_register(
    SRCS "src/HttpModule.cpp" "src/HttpModuleController.cpp" "src/Hardware/HardwareModule.cpp" "src/Wasm/WasmModule.cpp" "src/Preferences/PreferencesModule.cpp" "src/Network/NetworkModule.cpp"
    INCLUDE_DIRS "include"
    REQUIRES console HttpParser HttpServer HwController ArduinoJson WasmController)
