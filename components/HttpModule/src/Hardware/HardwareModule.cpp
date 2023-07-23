// #include "Hardware/HardwareModule.h"
#include "Hardware/HardwareModule.h"

namespace HardwareModule
{
    void HardwareModule::Get (HttpParser::HttpMessage* request, HttpParser::HttpMessage* response){
        ESP_LOGI("HardwareModule", "Request on path : %s", request->url.c_str());
        response->method = HttpParser::HttpMethod::NONE;
        ESP_LOGI("HardwareModule", "this->endpoint %s =?= %s", request->url.c_str(), this->endpoint.c_str());

        if(request->endpoint.size() == 0) return;
        else if(request->endpoint.size() == 1){
                response->method = HttpParser::HttpMethod::RESPONSE;
                response->content_type = HttpParser::HttpContentType::ApplicationJson;
                response->response_code = HttpParser::HttpResponseCode::OK;

                DynamicJsonDocument doc(2048*2);
                doc["idf_sdk_version"]  = "test";
                doc["mac_address"]      = 1;

                JsonObject chip_info = doc.createNestedObject("chip_info");
                chip_info["model"]      = "esp32";
                chip_info["features"]   = 1234;
                chip_info["revision"]   = 1;
                chip_info["cores"]      = 2;
                
                JsonObject memory = doc.createNestedObject("memory");
                memory["free_heap_size"]            = 1;
                memory["free_heap_size_minimum"]    = 1;
                
                std::string resp;
                serializeJson(doc, resp);
                size_t r = doc.overflowed();
                printf("%d, %s\n", r, resp.c_str());
                response->body = resp;
        }
        else if(request->endpoint.size() == 2){
            if(request->endpoint[1] == "box"){
                response->method = HttpParser::HttpMethod::RESPONSE;
                response->content_type = HttpParser::HttpContentType::ApplicationJson;
                response->response_code = HttpParser::HttpResponseCode::OK;

                DynamicJsonDocument doc(2048*2);
                doc["idf_sdk_version"]  = "test";
                doc["mac_address"]      = 1;

                JsonObject chip_info = doc.createNestedObject("chip_info");
                chip_info["model"]      = "esp32";
                chip_info["features"]   = 1234;
                chip_info["revision"]   = 1;
                chip_info["cores"]      = 2;
                
                JsonObject memory = doc.createNestedObject("memory");
                memory["free_heap_size"]            = 1;
                memory["free_heap_size_minimum"]    = 1;
                
                std::string resp;
                serializeJson(doc, resp);
                size_t r = doc.overflowed();
                printf("%d, %s\n", r, resp.c_str());
                response->body = resp;
            }
        }

    }
    void HardwareModule::Post(HttpParser::HttpMessage* request, HttpParser::HttpMessage* response){
        
    }
}