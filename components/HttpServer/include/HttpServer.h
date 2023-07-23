#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "esp_err.h"
#include "HttpParser.h"
#include "NetworkController.h"

namespace HttpServer{

    class SimpleHttpServerBase : NetworkController::TcpTranscieverBase {
        public:
            NetworkController::TcpTranscieverResponse operator()(NetworkController::tcp_transceiver_task_context_t* context);
    };

    class HttpServerBase : NetworkController::TcpRawTranscieverBase {
        private:
            // int empty_newline_count = 0;
            // std::string request = "";
        public:
            virtual HttpParser::HttpMessage* serve(HttpParser::HttpMessage* request);
            NetworkController::TcpTranscieverResponse operator()(NetworkController::tcp_raw_transceiver_task_context_t* context);
    };
}