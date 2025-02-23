#pragma once

#include "FApplication.h"
#include "FLog.h"
#include "Types.h"

extern App *create_application();

inline s32 stak_main() {
    Log::init();

    App *app = create_application();
    app->run();
    delete app;

    return 0;
}

int main(int argc, char **argv) {
    return stak_main();
}

