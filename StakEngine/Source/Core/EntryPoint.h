#pragma once

#include "Application.h"
#include "Log.h"
#include "Types.h"

extern bool InitializeApplication(FApplication *App);

extern FApplication *GApplication;

inline FSInt32 StakMain() {
    FLog::Init();

    auto App = new FApplication;

    if (!InitializeApplication(App)) {
        SK_LOG_ERROR("Failed to initialize application");
        return -1;
    }

    App->Run();

    delete App;

    return 0;
}

int main(int argc, char **argv) {
    return StakMain();
}

