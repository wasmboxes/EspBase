#include "HttpModule.h"
#include "HwController.h"
#include "ArduinoJson.h"


namespace HardwareModule
{
    class HardwareModule : public HttpModule::HttpModuleBase{
        public:
            
            HwController* hwController;
            void Get (HttpParser::HttpMessage* request, HttpParser::HttpMessage* response);
            void Post(HttpParser::HttpMessage* request, HttpParser::HttpMessage* response);
    };
} // namespace HardwareModule 
