#include "Application.h"

#include "Log.h"
#include "Asserts.h"
#include "RHI.h"
#include "Platform.h"

void FEngine::Shutdown() {
    App->Shutdown(this, App);

    ImGuiRenderer.Shutdown();

    PlatformCloseWindow(&Window);

    Renderer.Shutdown();

    RHIShutdown();

    PlatformShutdown();
}

bool FEngine::Init(FEngineSpecification *Spec, FApplication *AppImpl) {
    App = AppImpl;

    Name = Spec->AppName;

    if (!PlatformInit()) {
        SK_LOG_ERROR("Failed to initialize platform");
        return false;
    }

    if (!RHIInit()) {
        SK_LOG_ERROR("Failed to initialize RHI");
        return false;
    }

    FWindowConfig Cfg = {
        Spec->WindowWidth,
        Spec->WindowHeight,
        Spec->WindowTitle,
        ([this](FEvent *Event) {
            this->OnEvent(Event);
        })
    };

    if (!PlatformOpenWindow(&Window, &Cfg)) {
        SK_LOG_ERROR("Failed to create window");
        return false;
    }

    assert(InitializeInput(&Input, &Window));

    if (!Renderer.Init(&Window, Spec->RenderToOffscreenBuffer)) {
        SK_LOG_ERROR("Failed to initialize renderer");
        return false;
    }

    ImGuiRenderer.Init(&Renderer);

    Running = true;

    if (App) App->Init(this, App);

    return true;
}

void FEngine::Run() {
    while (Running) {
        PlatformProcessMessages();

        if (App->Update) App->Update(this, App);

        ImGuiRenderer.BeginFrame();
        if (App->OnImGuiRender) App->OnImGuiRender(this, App);
        ImGuiRenderer.EndFrame();

        if (!Renderer.Render()) {
            SK_LOG_ERROR("Failed to render");
        }
    }
}

void FEngine::OnEvent(FEvent *Event) {
    switch (Event->Type) {
    case EEventType::WINDOW_CLOSE:
        Running = false;
        break;
    case EEventType::KEY:
        SetKey(&Input, Event->KE.Key, Event->KE.State);
        break;
    case EEventType::MOUSE_BUTTON:
        SetMouseButton(&Input, Event->MBE.Button, Event->MBE.State);
        break;
    case EEventType::MOUSE_MOVE:
        SetMousePos(&Input, Event->MME.X, Event->MME.Y);
        break;
    default:
        break;
    }

    if (App->OnEvent) App->OnEvent(this, Event);
}

void FEngine::Close() {
    Running = false;
}