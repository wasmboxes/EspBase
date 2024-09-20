#include "Wasm/WasmModule.h"

namespace WasmModule
{
    void WasmModule::Get (HttpParser::HttpMessage* request, HttpParser::HttpMessage* response){
        ESP_LOGI("WasmModule", "Get Request on path : %s", request->url.c_str());
        response->method = HttpParser::HttpMethod::NONE;

        if(request->endpoint.size() == 0) return;
        else if(request->endpoint.size() == 1) return;
        else if(request->endpoint.size() == 2){

            if(request->endpoint[1] == "box"){
                response->method = HttpParser::HttpMethod::RESPONSE;
                response->content_type = HttpParser::HttpContentType::ApplicationJson;
                response->response_code = HttpParser::HttpResponseCode::OK;

                 printf("\nFree heap : %d\n", esp_get_free_heap_size());
                std::string resp = "";

                {
                    DynamicJsonDocument document(2048*2);
                    // if(document.isNull()) //  printf("\n doc is null\n");
                    size_t r = document.overflowed();
                    printf(" overflowed : %d\n", r);
                    printf("\nFree heap : %d\n", esp_get_free_heap_size());
                    
                    if(this->wasmController != nullptr){
                        for(const auto& box : this->wasmController->Sandboxes){
                            JsonObject js_box = document.createNestedObject();
                            if(js_box.isNull()) //  printf("\njsbox is null\n");
                            // //  printf("Serializing box id : %d , %s , %d\n",box.second->id, box.second->moduleName, box.second->error);
                            js_box["Id"]      = box.second->id;
                            js_box["Name"]    = box.second->moduleName;
                            js_box["ErrorId"] = box.second->error;
                        }
                    }
                    serializeJson(document, resp);
                    document.clear();
                    document.garbageCollect();
                }
                
                 printf("\nFree heap : %d\n", esp_get_free_heap_size());
                response->body = resp;
            }
            if(request->endpoint[1] == "list"){
                response->method = HttpParser::HttpMethod::RESPONSE;
                response->content_type = HttpParser::HttpContentType::ApplicationJson;
                response->response_code = HttpParser::HttpResponseCode::OK;

                 printf("\nFree heap : %d\n", esp_get_free_heap_size());
                std::string resp = "";

                {
                    DynamicJsonDocument document(2048*2);
                    if(document.isNull()) 
                    size_t r = document.overflowed();
                    
                    if(this->wasmController != nullptr){
                        for(const auto& box : this->wasmController->Sandboxes){
                            JsonObject js_box = document.createNestedObject();
                            if(js_box.isNull()) //  printf("\njsbox is null\n");
                            // //  printf("Serializing box id : %d , %s , %d\n",box.second->id, box.second->moduleName, box.second->error);
                            js_box["Id"]                = box.second->id;
                            js_box["Name"]              = box.second->moduleName;
                            js_box["ErrorId"]           = box.second->error;
                            js_box["FunctionCount"]     = (unsigned int)0;
                            js_box["ModuleCount"]       = (unsigned int)0;

                            JsonArray js_functionArray = js_box.createNestedArray("Functions");//js_box["Functions"].createNestedArray();

                            // trebao bi biti samo jedan modul u runtime-u....zasad
                            IM3Module module = box.second->runtime->modules;
                            int moduleCount = 0;
                            while (module)
                            {
                                moduleCount ++;
                                js_box["FunctionCount"] = module->numFunctions;

                                IM3Function function = module->functions;
                                //  printf("\nNumber of funcs : %d\n", module->numFunctions);
                                for(int fi = 0; fi < module->numFunctions; fi++){
                                    JsonObject js_functionObject = js_functionArray.createNestedObject();
                                    js_functionObject["Index"] = function[fi].index;
                                    if(function[fi].names[0] != nullptr) // function[fi].names != nullptr && 
                                        js_functionObject["Name"] = function[fi].names[0];
                                    else js_functionObject["Name"] = "";

                                    IM3FuncType functionType = function[fi].funcType;//->funcType;

                                    while(functionType){
                                        js_functionObject["ArgCount"] = functionType->numArgs;
                                        //  printf("Number of args : %d\n", functionType->numArgs);
                                        js_functionObject["RetCount"] = functionType->numRets;
                                        JsonArray js_functionType = js_functionObject.createNestedArray("Types");
                                        int maxElems = functionType->numArgs + functionType->numRets;
                                        for(int ei = 0; ei < maxElems; ei ++){
                                            js_functionType.add(functionType->types[ei]);
                                        }
                                        
                                        // functionType = functionType->next; // trebao bi biti samo jedan tip ?
                                        functionType = nullptr; // samo prvi tip uzimamo
                                    }
                                }
                                module = module->next;
                            }
                            js_box["ModuleCount"] = moduleCount;
                            
                        }
                    }
                    serializeJson(document, resp);
                    document.clear();
                    document.garbageCollect();
                }
                //  printf("\nFree heap : %d\n", esp_get_free_heap_size());
                response->body = resp;
            }
        }

    }

    void WasmModule::Post(HttpParser::HttpMessage* request, HttpParser::HttpMessage* response){
        ESP_LOGI("WasmModule", "Post Request on path : %s", request->url.c_str());
        response->method = HttpParser::HttpMethod::NONE;
        ESP_LOGI("WasmModule", "Post Request body : %s", request->body.c_str());
        
        if(request->endpoint.size() == 0) return;
        else if(request->endpoint.size() == 1) return;
        else if(request->endpoint.size() == 2){

            if(request->endpoint[1] == "box"){
                response->method = HttpParser::HttpMethod::RESPONSE;
                response->content_type = HttpParser::HttpContentType::ApplicationJson;
                response->response_code = HttpParser::HttpResponseCode::OK;

                DynamicJsonDocument doc(2048*4);
                DeserializationError err = deserializeJson(doc, request->body);
                std::string resp;
                serializeJson(doc, resp);
                size_t r = doc.overflowed();
                response->body = resp;
            }
            else if(request->endpoint[1] == "run"){
                response->method = HttpParser::HttpMethod::RESPONSE;
                response->content_type = HttpParser::HttpContentType::ApplicationJson;
                response->response_code = HttpParser::HttpResponseCode::OK;
                
                DynamicJsonDocument docResp(2048*2);
                JsonArray jsonResponseArray = docResp.createNestedArray("Response");
                docResp["Error"]     = 0;
                docResp["ErrorDesc"] = "";
                std::string resp;

                DynamicJsonDocument doc(2048*4);
                DeserializationError err = deserializeJson(doc, request->body);
                if(err.code() != 0){
                    docResp["ErrorDesc"] = "Deserialization error";
                    docResp["Error"]     = err.code();
                }
                else{

                    if(doc.containsKey("BoxId") && doc.containsKey("FunctionName") && doc.containsKey("Params")){
                        if(doc["BoxId"].is<int>() && doc["FunctionName"].is<const char*>() && doc["Params"].is<JsonArray>()){
                            
                            int boxId = doc["BoxId"].as<int>();
                            const char* functionName = doc["FunctionName"].as<const char*>();
                            if(this->wasmController->SandboxHasFunction(boxId, functionName) == WasmController::WASM_STATUS::OK){
                                IM3Function function;
                                uint32_t argc = doc["Params"].as<JsonArray>().size();
                                JsonArray paramsArray = doc["Params"].as<JsonArray>();
                                const void *argptrp[argc];

                                for(int i = 0; i < argc; i++){
                                    if(paramsArray[i].containsKey("Value") && paramsArray[i].containsKey("Ptype")){
                                        switch(paramsArray[i]["Ptype"].as<int>()){
                                            case 1: // INT32
                                                *((int*)argptrp[i]) = (int)paramsArray[i]["Value"].as<int>();
                                                break;
                                            case 2: // INT64
                                                *((long int*)argptrp[i]) = (long int)paramsArray[i]["Value"].as<long int>();
                                                break;
                                            case 3: // FLT32
                                                *((float*)argptrp[i]) = (float)paramsArray[i]["Value"].as<float>();
                                                break;
                                            case 4: // FLT64
                                                *((double*)argptrp[i]) = (double)paramsArray[i]["Value"].as<double>();
                                                break;
                                            case 5: // PNTR
                                                *((char*)argptrp[i]) = *(const char*)paramsArray[i]["Value"].as<const char*>();
                                                break;
                                            default:
                                                argptrp[i] = nullptr;
                                                break;
                                        }
                                    }
                                }

                                WasmController::WASM_STATUS status = this->wasmController->SandboxCallFunction(boxId, functionName, function, argc, argptrp);

                                if(status == WasmController::OK){
                                    this->wasmController->SandboxGetResults(function, jsonResponseArray);
                                    unsigned value = 0;
                                    this->wasmController->SandboxGetResults(function, &value);
                                    jsonResponseArray.add(value);
                                }
                                else {
                                    docResp["ErrorDesc"] = "Error function call error";
                                    docResp["Error"]     = -1;
                                }    

                                this->wasmController->SandboxFree(boxId);
                            }
                            else {
                                docResp["ErrorDesc"] = "Error function not found";
                                docResp["Error"]     = -1;
                            }
                        }
                        else{
                            docResp["ErrorDesc"] = "Error message wrong format";
                            docResp["Error"]     = 1;
                        }
                    }
                }

                serializeJson(docResp, resp);
                size_t r = docResp.overflowed();
                if(r != 0){
                    response->body = "0";
                }else{
                    response->body = resp;
                }
            }
            

        }

    }








} // namespace WasmModule 
