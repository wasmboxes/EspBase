#include "HttpModuleController.h"

namespace HttpModule
{
    
    HttpParser::HttpMessage* HttpModuleController::serve(HttpParser::HttpMessage* request){
        HttpParser::HttpMessage* response = NULL;
        response = new HttpParser::HttpMessage();
        response->method = HttpParser::HttpMethod::RESPONSE;
        response->method = HttpParser::HttpMethod::NONE;

        ESP_LOGI("HttpServerBase", "Request on path : %s", request->url.c_str());

        for(const auto& module : this->modules) {

            if(response->method != HttpParser::HttpMethod::NONE || response->error != HttpParser::HttpMessageError::None)        break;

            // if(request->endpoint.size() > 0) ESP_LOGI("HttpServerBase", "Checking modules for endpoint %s", request->endpoint[0].c_str());
            
            if(request->endpoint.size() > 0 && request->endpoint[0] == module.second->endpoint){
                if(request->method == HttpParser::HttpMethod::GET)          module.second->Get(request, response);
                else if(request->method == HttpParser::HttpMethod::POST)    module.second->Post(request, response);
            }
        }       
        if(response->method == HttpParser::HttpMethod::NONE){
            response->method = HttpParser::HttpMethod::RESPONSE;
            response->body = "<html><body><h1>Error Not Found</h1></body></html>";
            response->response_code = HttpParser::HttpResponseCode::NOTFOUND;
        }

        // response->method = HttpParser::HttpMethod::RESPONSE;
        // response->body = "<html><body><h1>Error Not Found</h1></body></html>";
        // response->response_code = HttpParser::HttpResponseCode::NOTFOUND;
        
        return response;
    }

    void HttpModuleController::AddModule(HttpModuleBase* module){
        this->modules[module->id] = module;
    }

    void HttpModuleController::RemoveModule(int module_id){
        this->modules.erase(module_id);
    }

} // namespace HttpModule
