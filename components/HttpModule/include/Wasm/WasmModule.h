#include "HttpModule.h"
#include "WasmController.h"
#include "ArduinoJson.h"
#include <mutex>


namespace WasmModule
{
    class WasmModule : public HttpModule::HttpModuleBase{
        public:
            
            WasmController::WasmController* wasmController = nullptr;
            void Get (HttpParser::HttpMessage* request, HttpParser::HttpMessage* response);
            void Post(HttpParser::HttpMessage* request, HttpParser::HttpMessage* response);
    };
} // namespace WasmModule 
