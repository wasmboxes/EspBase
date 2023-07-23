#pragma once
#include <stdio.h>
#include "esp_err.h"

#include "HttpParser.h"
#include "HttpServer.h"
#include "HttpModule.h"

namespace HttpModule{


    class HttpModuleController : HttpServer::HttpServerBase{
        private:
            std::map<int,HttpModuleBase*> modules;
        
        public:
            HttpParser::HttpMessage* serve(HttpParser::HttpMessage* request);

            void AddModule(HttpModuleBase* module);
            void RemoveModule(int module_id);
    };

}