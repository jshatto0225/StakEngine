#include "Engine.h"

#include "Log.h"
#include "Asserts.h"
#include "RHI.h"
#include "Platform.h"

void EngineShutdown(FEngine *Engine) {
    Engine->App->Shutdown(Engine, Engine->App);

    ImGuiRendererShutdown(&Engine->ImGuiRenderer);

    PlatformCloseWindow(&Engine->Window);

    RendererShutdown(&Engine->Renderer);

    RHIShutdown();

    PlatformShutdown();
}

bool EngineInit(FEngine *Engine, FEngineSpecification *Spec, FApplication *AppImpl) {
    Engine->App = AppImpl;

    Engine->Name = Spec->AppName;

    if (!PlatformInit()) {
        SK_LOG_ERROR("Failed to initialize platform");
        return false;
    }

    if (!RHIInit(ERHIBackend::VULKAN)) {
        SK_LOG_ERROR("Failed to initialize RHI");
        return false;
    }

    FWindowConfig Cfg = {
        Spec->WindowWidth,
        Spec->WindowHeight,
        Spec->WindowTitle,
        ([Engine](FEvent *Event) {
            EngineOnEvent(Engine, Event);
        })
    };

    if (!PlatformOpenWindow(&Engine->Window, &Cfg)) {
        SK_LOG_ERROR("Failed to create window");
        return false;
    }

    assert(InitializeInput(&Engine->Input, &Engine->Window));

    if (!RendererInit(&Engine->Renderer, &Engine->Window, Spec->RenderToOffscreenBuffer)) {
        SK_LOG_ERROR("Failed to initialize renderer");
        return false;
    }

    ImGuiRendererInit(&Engine->ImGuiRenderer, &Engine->Renderer);

    Engine->Running = true;

    if (Engine->App) {
        if (!Engine->App->Init(Engine, Engine->App)) {
            SK_LOG_ERROR("Failed to initialize application");
            return false;
        }
    }

    return true;
}

void EngineRun(FEngine *Engine) {
    while (Engine->Running) {
        PlatformProcessMessages();

        if (Engine->App->Update) Engine->App->Update(Engine, Engine->App);

        ImGuiRendererBeginFrame(&Engine->ImGuiRenderer);
        if (Engine->App->OnImGuiRender) Engine->App->OnImGuiRender(Engine, Engine->App);
        ImGuiRendererEndFrame(&Engine->ImGuiRenderer);

        if (!RendererRender(&Engine->Renderer)) {
            SK_LOG_ERROR("Failed to render");
        }
    }
}

void EngineOnEvent(FEngine *Engine, FEvent *Event) {
    switch (Event->Type) {
    case EEventType::WINDOW_CLOSE:
        Engine->Running = false;
        break;
    case EEventType::KEY:
        SetKey(&Engine->Input, Event->KE.Key, Event->KE.State);
        break;
    case EEventType::MOUSE_BUTTON:
        SetMouseButton(&Engine->Input, Event->MBE.Button, Event->MBE.State);
        break;
    case EEventType::MOUSE_MOVE:
        SetMousePos(&Engine->Input, Event->MME.X, Event->MME.Y);
        break;
    default:
        break;
    }

    if (Engine->App->OnEvent) Engine->App->OnEvent(Engine, Event);
}

void EngineClose(FEngine *Engine) {
    Engine->Running = false;
}