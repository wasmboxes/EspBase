#include "HttpModule.h"
#include "HwController.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"
#include "rapidjson.h"
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
