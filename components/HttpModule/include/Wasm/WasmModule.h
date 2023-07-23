#include "HttpModule.h"
#include "WasmController.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"
#include "rapidjson.h"
#include "ArduinoJson.h"
#include <mutex>


namespace WasmModule
{
    class WasmModule : public HttpModule::HttpModuleBase{
        public:
            
            WasmController::WasmController* wasmController = nullptr;
            void Get (HttpParser::HttpMessage* request, HttpParser::HttpMessage* response);
            void Post(HttpParser::HttpMessage* request, HttpParser::HttpMessage* response);
            // DynamicJsonDocument document(2048);
            // StaticJsonDocument<4096> document;
            // std::mutex documentMutex;
    };
} // namespace WasmModule 
