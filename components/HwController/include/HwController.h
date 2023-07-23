#pragma once
#include "PreferencesController.h"
#include <stdio.h>
#include "esp_err.h"

// #ifdef __cplusplus
// extern "C" {
// #endif

class HwController
{
private:

public:
    Preferences_::PreferencesController* preferencesController = NULL;
    HwController(Preferences_::PreferencesController* preferencesController);

    void Boot();

    ~HwController();
};

// #ifdef __cplusplus
// }
// #endif
