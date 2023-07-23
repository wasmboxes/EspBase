idf_component_register(SRCS "src/HttpServer.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES console HttpParser NetworkController HttpParser)
