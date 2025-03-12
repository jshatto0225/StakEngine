#pragma once

#include "Application.h"
#include "Log.h"
#include "Types.h"

extern void InitializeApplication(FApplication *OutApp, FEngineSpecification *OutSpec);
extern void DestroyApplication(FApplication *App);

extern FEngine *GEngine;

inline FSInt32 StakMain() {
    LogInit();

    FEngine Engine = {};
    FApplication App = {};
    FEngineSpecification Spec = {};

    InitializeApplication(&App, &Spec);

    if (!Engine.Init(&Spec, &App)) {
        SK_LOG_CRITICAL("Failed to initialize engine");
        return -1;
    }

    Engine.Run();

    Engine.Shutdown();

    DestroyApplication(Engine.App);

    return 0;
}

int main(int argc, char **argv) {
    return StakMain();
}

