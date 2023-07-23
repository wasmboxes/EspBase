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


#define _FATAL(msg, ...) { printf("Fatal: " msg "\n", ##__VA_ARGS__); return 0; }

namespace WasmController{

    const char* TAG = "WasmController";

    int WasmController::GetId(){
        this->current_id += 1;
        return this->current_id;
    }

    void WasmController::RemoveId(){
        return; // TODO
    }

    int WasmController::LinkDefaultFunctions(WasmSandbox* box){
        return 1; // TODO
    }

    unsigned int WasmController::SandboxCreate(WasmSandbox* box, unsigned char* code, int code_len, const char* moduleName){
        // WasmSandbox* box = new WasmSandbox();
        // sandbox = box;
        box->error = false;
        box->moduleName = moduleName;
        box->id = this->GetId();
        box->code = code;
        box->code_len = code_len;
        
        this->SandboxAdd(box);

        this->SandboxCleanLoad(box);

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

        // IM3Function f;
        // this->SandboxGetFunction(box->id, "test", f);

        return true;
    }


    int WasmController::SandboxAdd(WasmSandbox* box){
        this->Sandboxes[box->id] = box; 
        return 1;
    }

    void WasmController::SandboxRemove(int box_id){
        this->Sandboxes.erase(box_id); // TODO check if i should check if it exists
        return ;
    }

    WASM_STATUS WasmController::SandboxGetFunction(int box_id, const char* name, IM3Function &function){
        M3Result result = m3Err_none;
        if(this->Sandboxes.count(box_id) == 0) return ERROR;
        result = m3_FindFunction (&function, this->Sandboxes[box_id]->runtime, name);
        if(result){
            printf("m3_GetResults: %s for function %s\n", result, name);
            return WASM_STATUS::NOT_FOUND;
        }
        ESP_LOGI(TAG,"Get function %s ok", name);
        return WASM_STATUS::OK;
    }

    WASM_STATUS WasmController::SandboxHasFunction(int box_id, const char* name){
        if(this->Sandboxes.count(box_id) == 0) return ERROR;
        M3Result result = m3Err_none;
        IM3Function f;
        result = m3_FindFunction (&f, this->Sandboxes[box_id]->runtime, name);
        if(result){
            printf("m3_GetResults: %s for function %s\n", result, name);
            return WASM_STATUS::NOT_FOUND;
        }
        // Function_Release(f);
        ESP_LOGI(TAG,"Has function %s ok", name);
        return WASM_STATUS::OK;
    }

    WASM_STATUS WasmController::SandboxCallFunctionV(int box_id, const char* name, IM3Function &function, ...){
        if(this->Sandboxes.count(box_id) == 0) return ERROR;
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
        if(this->Sandboxes.count(box_id) == 0) return ERROR;
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
        m3_FreeRuntime(this->Sandboxes[box_id]->runtime);
        m3_FreeEnvironment(this->Sandboxes[box_id]->env);
        // m3_FreeModule(this->Sandboxes[box_id]->module);
        // M3Result result = m3_LinkRawFunction(this->Sandboxes[box_id]->module, module_name, function_name, signature, function);
        // ESP_LOGI(TAG,"Linking function %s to module %s with id %d result : %s", function_name, module_name, box_id, result);
        this->SandboxCleanLoad(this->Sandboxes[box_id]);
        return WASM_STATUS::OK;
    }


    WasmController::WasmController(/* args */)
    {
    }

    WasmController::~WasmController()
    {
    }

}