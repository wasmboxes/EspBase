#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "WasmController.h"
#include <wasm3.h>
#include <m3_env.h>
#include "esp_event.h"
#include "esp_log.h"
#include "BoxDevice.h"

#define _FATAL(msg, ...) { printf("Fatal: " msg "\n", ##__VA_ARGS__); return 0; }




namespace WasmController{

    const char* TAG = "WasmController";
    WasmController* WasmController::instance = nullptr;

    m3ApiRawFunction(m3_DevicePutInt32){
        m3ApiGetArg     (uint32_t, id)
        m3ApiGetArg     (uint32_t, value)
        WasmController* instance = (new WasmController())->GetInstance();
        instance->GetDevices()->PutUInt32(id, value);
        m3ApiSuccess();
    }
    
    m3ApiRawFunction(m3_DeviceGetInt32){
        m3ApiGetArg     (uint32_t, id)
        m3ApiReturnType (uint32_t)
        WasmController* instance = (new WasmController())->GetInstance();
        m3ApiReturn(instance->GetDevices()->GetUInt32(id));
    }

    m3ApiRawFunction(m3_delay){
        m3ApiGetArg     (uint32_t, ms)
        vTaskDelay(ms / portTICK_PERIOD_MS);
        m3ApiSuccess();
    }

    m3ApiRawFunction(m3_getRandom){
        m3ApiReturnType (uint32_t)
        m3ApiReturn(89);
    }

    int WasmController::GetId(){
        WasmController::instance->current_id += 1;
        return WasmController::instance->current_id;
    }

    void WasmController::RemoveId(){
        return; // TODO
    }

    int WasmController::LinkDefaultFunctions(WasmSandbox* box){
        ESP_LOGI(TAG,"Linking default functions");
        char* module_name = "env";

        m3_LinkRawFunction (box->runtime->modules, "env", "millis", "v(i)", &m3_delay);
        m3_LinkRawFunction (box->runtime->modules, "env", "getRandom", "i()", &m3_getRandom);
        
        m3_LinkRawFunction (box->runtime->modules, "env", "DevicePutInt32", "v(ii)", &m3_DevicePutInt32);
        m3_LinkRawFunction (box->runtime->modules, "env", "DeviceGetInt32", "i(i)", &m3_DeviceGetInt32);

        // WasmController::instance->SandboxLinkRawFuntion(box->id, m3_getRandom, module_name, "getRandom", "i()");
        // WasmController::instance->SandboxLinkRawFuntion(box->id, m3_delay, box->moduleName, "delay", "v(i)");
        // m3_LinkRawFunction (box->module, "", "millis",           "i()",    &m3_delay);

        return 1;
    }

    unsigned int WasmController::SandboxCreate(WasmSandbox* box, unsigned char* code, int code_len, const char* moduleName){
        // WasmSandbox* box = new WasmSandbox();
        // sandbox = box;
        box->error = false;
        box->moduleName = moduleName;
        box->id = WasmController::instance->GetId();
        box->code = code;
        box->code_len = code_len;
        
        WasmController::instance->SandboxAdd(box);

        WasmController::instance->SandboxCleanLoad(box);

        return true;
    }

    unsigned int WasmController::SandboxCleanLoad(WasmSandbox* box){
        M3Result result = m3Err_none;
        
       IM3Environment env = m3_NewEnvironment ();
        if(!env){
            ESP_LOGI(TAG,"Environment error");
            box->error = true;
            _FATAL("m3_NewEnvironment failed");
        }

        IM3Runtime runtime = m3_NewRuntime (env, WASM_STACK_SLOTS, NULL);
        if(!runtime){
            ESP_LOGI(TAG,"Runtime error");
            box->error = true;
            _FATAL("m3_NewRuntime failed");
        }

        #ifdef WASM_MEMORY_LIMIT
            runtime->memoryLimit = WASM_MEMORY_LIMIT;
        #endif
        
        IM3Module module;

        box->env = env;
        box->runtime = runtime;
        box->module = module;

        result = m3_ParseModule (env, &module, (uint8_t*)box->code, box->code_len);
        if(result){
            ESP_LOGI(TAG,"Parse Module error");
            box->error = true;
            _FATAL("m3_ParseModule: %s", result);
        }
        
        result = m3_LoadModule (runtime, module);
        if(result){
            ESP_LOGI(TAG,"Load Module error");
            box->error = true;
            _FATAL("m3_LoadModule: %s", result);
        }

        this->LinkDefaultFunctions(box);

        return true;
    }

    int WasmController::SandboxAdd(WasmSandbox* box){
        WasmController::instance->Sandboxes[box->id] = box; 
        return 1;
    }

    void WasmController::SandboxRemove(int box_id){
        WasmController::instance->Sandboxes.erase(box_id); // TODO check if i should check if it exists
        return ;
    }

    WASM_STATUS WasmController::SandboxGetFunction(int box_id, const char* name, IM3Function &function){
        M3Result result = m3Err_none;
        if(WasmController::instance->Sandboxes.count(box_id) == 0) return ERROR;
        result = m3_FindFunction (&function, WasmController::instance->Sandboxes[box_id]->runtime, name);
        if(result){
            printf("m3_GetResults: %s for function %s\n", result, name);
            return WASM_STATUS::NOT_FOUND;
        }
        ESP_LOGI(TAG,"Get function %s ok", name);
        return WASM_STATUS::OK;
    }

    WASM_STATUS WasmController::SandboxHasFunction(int box_id, const char* name){
        if(WasmController::instance->Sandboxes.count(box_id) == 0) return ERROR;
        M3Result result = m3Err_none;
        IM3Function f;
        result = m3_FindFunction (&f, WasmController::instance->Sandboxes[box_id]->runtime, name);
        if(result){
            printf("m3_GetResults: %s for function %s\n", result, name);
            return WASM_STATUS::NOT_FOUND;
        }
        // Function_Release(f);
        ESP_LOGI(TAG,"Has function %s ok", name);
        return WASM_STATUS::OK;
    }

    WASM_STATUS WasmController::SandboxCallFunctionV(int box_id, const char* name, IM3Function &function, ...){
        if(WasmController::instance->Sandboxes.count(box_id) == 0) return ERROR;
        WASM_STATUS res = this->SandboxGetFunction(box_id, name, function);
        if(res != WASM_STATUS::OK) return res;
        va_list ap;
        va_start(ap, function);
        M3Result result = m3_CallVL(function, ap);
        va_end(ap);
        if(result){
            printf("m3_GetResults: %s", result);
            return WASM_STATUS::ERROR;
        }
        ESP_LOGI(TAG,"Calling function %s in module id %d", name, box_id);
        return WASM_STATUS::OK;
    }

    WASM_STATUS WasmController::SandboxCallFunction(int box_id, const char* name, IM3Function &function, uint32_t i_argc, const void * i_argptrs[]){
        if(WasmController::instance->Sandboxes.count(box_id) == 0) return ERROR;
        WASM_STATUS res = this->SandboxGetFunction(box_id, name, function);
        if(res != WASM_STATUS::OK) return res;
        M3Result result = m3_Call(function, i_argc, i_argptrs);
        if(result){
            printf("m3_GetResults: %s", result);
            return WASM_STATUS::ERROR;
        }
        ESP_LOGI(TAG,"Calling function %s in module id %d", name, box_id);
        return WASM_STATUS::OK;
    }

    WASM_STATUS WasmController::SandboxGetResults(IM3Function &function, ...){
        va_list ap;
        va_start(ap, function);
        M3Result result = m3_GetResultsVL(function, ap); 
        va_end(ap);
        // Function_Release(function);
        if(result) return WASM_STATUS::ERROR;
        return WASM_STATUS::OK;
    }

    WASM_STATUS WasmController::SandboxLinkRawFuntion(int box_id, M3RawCall function, const char* module_name,  const char* function_name, const char* signature){
        M3Result result = m3_LinkRawFunction(this->Sandboxes[box_id]->module, module_name, function_name, signature, function);
        ESP_LOGI(TAG,"Linking function %s to module %s with id %d result : %s", function_name, module_name, box_id, result);
        return WASM_STATUS::OK;
    }

    WASM_STATUS WasmController::SandboxFree(int box_id){
        m3_FreeRuntime(WasmController::instance->Sandboxes[box_id]->runtime);
        m3_FreeEnvironment(WasmController::instance->Sandboxes[box_id]->env);
        // m3_FreeModule(this->Sandboxes[box_id]->module);
        // M3Result result = m3_LinkRawFunction(this->Sandboxes[box_id]->module, module_name, function_name, signature, function);
        // ESP_LOGI(TAG,"Linking function %s to module %s with id %d result : %s", function_name, module_name, box_id, result);
        this->SandboxCleanLoad(WasmController::instance->Sandboxes[box_id]);
        return WASM_STATUS::OK;
    }

    void WasmController::LinkDevices(BoxDevice::BoxDevices* devices){
        this->devices = devices;
    }

    WasmController::WasmController(){
        if(WasmController::instance == nullptr){
            WasmController::instance = this;
        }
    }

    BoxDevice::BoxDevices* WasmController::GetDevices(){
        return this->devices;
    }

    WasmController* WasmController::GetInstance(){
        return WasmController::instance;
    }

    WasmController::~WasmController(){
    }

}