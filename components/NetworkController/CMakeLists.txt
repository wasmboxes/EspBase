idf_component_register(
    SRCS "src/NetworkController.cpp" "src/NetworkControllerTcp.cpp" "src/TcpTaskHandlers.cpp" "src/TcpTaskHandlerT.cpp" "src/TcpHandlers/Echo.cpp" "src/TcpHandlers/Echo.cpp" "src/TcpHandlers/RawTransceiver.cpp" "src/TcpHandlers/Transceiver.cpp" "src/TcpHandlers/Proxy.cpp"
    INCLUDE_DIRS "include" "include/TcpTaskHandlerT"
    REQUIRES mbedtls console spiffs nvs_flash PreferencesController)
