#pragma once

#include "Engine.h"
#include "Log.h"
#include "Types.h"

extern void initialize_application(Application *out_app, EngineSpecification *out_spec);
extern void destroy_application(Application *app);

extern Engine *global_engine;

inline s32 stak_main() {
    log_init();

    Engine engine            = {};
    Application app          = {};
    EngineSpecification spec = {};

    initialize_application(&app, &spec);

    if (!engine_init(&engine, &spec, &app)) {
        SK_LOG_CRITICAL("Failed to initialize engine");
        return -1;
    }

    engine_run(&engine);

    engine_shutdown(&engine);

    destroy_application(engine.app);

    return 0;
}

int main(int argc, char **argv) {
    return stak_main();
}

