#pragma once

#include "Engine.h"
#include "Log.h"
#include "Types.h"

extern Application *create_application();
extern void destroy_application(Application *app, Engine *engine);

inline s32 stak_main() {
    log_init();

    Engine *engine = create_engine();
    if (!engine) {
        SK_LOG_ERROR("stak_main create_engine failed");
        return -1;
    }

    Application *app = create_application();
    if (!app) {
        SK_LOG_ERROR("stak_main create_application failed");
        return -1;
    }

    EngineSpecification spec = app->get_engine_spec(app);

    if (!engine_init(engine, &spec, app)) {
        SK_LOG_CRITICAL("stak_main engine_init failed");
        return -1;
    }

    engine_run(engine);

    destroy_application(app, engine);

    destroy_engine(engine);

    log_shutdown();

    return 0;
}

int main(int argc, char **argv) {
    return stak_main();
}

