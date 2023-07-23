#pragma once
#include <stdio.h>
#include "esp_err.h"
#include "HttpParser.h"

namespace HttpModule{
    class HttpModuleBase{
        private:
        public:
            int id = -1;
            std::string endpoint;
            virtual void Get(HttpParser::HttpMessage* request, HttpParser::HttpMessage* response);
            virtual void Post(HttpParser::HttpMessage* request, HttpParser::HttpMessage* response);
    };
}