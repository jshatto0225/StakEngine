#pragma once

#include "Application.h"
#include "Log.h"
#include "Types.h"

extern FApplication *CreateApplication();

extern FApplication *GApplication;

inline FSInt32 StakMain() {
    FLog::Init();

    auto App = CreateApplication();

    AppRun();

    delete App;

    return 0;
}

int main(int argc, char **argv) {
    return StakMain();
}

