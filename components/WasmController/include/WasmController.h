#pragma once
#include "PreferencesController.h"
#include <stdio.h>
#include "esp_err.h"
#include <wasm3.h>
#include <m3_env.h>
#include <map>
#include "BoxDevice.h"

namespace WasmController{
#define WASM_STACK_SLOTS    1024 * 4
//#define NATIVE_STACK_SIZE   (32*1024)
#define WASM_MEMORY_LIMIT   4096 * 4

/*
 * API bindings
 *
 * Note: each RawFunction should complete with one of these calls:
 *   m3ApiReturn(val)   - Returns a value
 *   m3ApiSuccess()     - Returns void (and no traps)
 *   m3ApiTrap(trap)    - Returns a trap
 */

// #ifdef __cplusplus
// extern "C" {
// #endif
// const void *WasmController::m3_arduino_millis(IM3Runtime runtime, IM3ImportContext _ctx, uint64_t *_sp, void *_mem)

    extern const char *TAG;

    enum WASM_STATUS{
        OK          = 1,
        ERROR       = 0,
        NOT_FOUND   = 20
    };

    struct WasmSandbox{
        int id;
        const char* moduleName; // \0 terminated
        IM3Environment env;
        IM3Runtime runtime;
        IM3Module module;
        unsigned char* code;
        int code_len;
        bool error;
    };

    class WasmController
    {
    private:
        // void register 

        static WasmController* instance;
        BoxDevice::BoxDevices* devices = nullptr;
        int LinkDefaultFunctions(WasmSandbox* box);
        int current_id = 0;

        int GetId();
        void RemoveId();

    public:
        WasmController();

        std::map<int, WasmSandbox*> Sandboxes; // ID of sandbox, sandbox


        unsigned int SandboxCreate(WasmSandbox* sandbox, unsigned char* code, int code_len, const char* moduleName);
        int SandboxAdd(WasmSandbox* box); // returns id
        void SandboxRemove(int box_id); // ! deallocate WasmSandbox
        unsigned int SandboxCleanLoad(WasmSandbox* box);

        WASM_STATUS SandboxGetFunction(int box_id, const char* name, IM3Function &function);
        WASM_STATUS SandboxHasFunction(int box_id, const char* name);
        WASM_STATUS SandboxCallFunctionV(int box_id, const char* name, IM3Function &function, ...);
        WASM_STATUS SandboxCallFunction(int box_id, const char* name, IM3Function &function, uint32_t i_argc, const void * i_argptrs[]);
        WASM_STATUS SandboxGetResults(IM3Function &function, ...);
        // M3Result SandboxCallFunction(IM3Function function, ...);
        WASM_STATUS SandboxLinkRawFuntion(int box_id, M3RawCall function, const char* module_name,  const char* function_name, const char* signature);
        WASM_STATUS SandboxFree(int box_id);

        void LinkDevices(BoxDevice::BoxDevices* devices);
        BoxDevice::BoxDevices* GetDevices();

        static WasmController* GetInstance();

        ~WasmController();
    };




// #ifdef __cplusplus
// }
// #endif
}