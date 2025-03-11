#include "Application.h"

#include "Log.h"
#include "ImGuiLayer.h"

#include "Asserts.h"
#include "RHI.h"
#include "Platform.h"

FApplication *GApplication = nullptr;

void FApplication::AddLayer(IApplicationLayer *Layer) {
    LayerStack.Push(Layer);
}

void FApplication::Shutdown() {
    LayerStack.Clear();

    Renderer.Shutdown();

    Window->Shutdown();

    RHIShutdown();

    if (GApplication == this) {
        GApplication = nullptr;
    }
}

bool FApplication::Init(const FApplicationSpec &Spec) {
    if (!GApplication) {
        GApplication = this;
    }

    Name = Spec.AppName;

    if (!RHIInit()) {
        SK_LOG_ERROR("Failed to initialize RHI");
        return false;
    }

    FWindowConfig Cfg = {
        Spec.WindowWidth,
        Spec.WindowHeight,
        Spec.WindowTitle
    };

    Window = IWindow::Create();
    if (!Window->Init(Cfg)) {
        SK_LOG_ERROR("Failed to create window");
        return false;
    }

    Window->SetEventFn([this](const FEvent &Event) {
        this->OnEvent(Event);
    });

    Input = TCreateRef<FInput>(Window);

    if (!Renderer.Init(Window, Spec.RenderToOffscreenBuffer)) {
        SK_LOG_ERROR("Failed to initialize renderer");
        return false;
    }

    ImGuiLayer = new FImGuiLayer();

    AddLayer(ImGuiLayer);

    Running = true;

    return true;
}

void FApplication::Run() {
    while (Running) {
        PlatformPollEvents();

        for (IApplicationLayer *Layer : LayerStack) {
            Layer->Update();
        }

        Renderer.ImGuiNewFrame();
        ImGuiLayer->BeginFrame();
        {
            for (IApplicationLayer *Layer : LayerStack) {
                Layer->OnImGuiRender();
            }
        }
        ImGuiLayer->EndFrame();

        if (!Renderer.Render()) {
            SK_LOG_ERROR("Failed to render");
        }
    }
}

void FApplication::OnEvent(const FEvent &Event) {
    switch (Event.Type) {
    case EEventType::WINDOW_CLOSE:
        Running = false;
        break;
    default:
        break;
    }


}

void FApplication::Close() {
    Running = false;
}

void AppRun() {
    GApplication->Run();
}

void AppAddLayer(IApplicationLayer *layer) {
    GApplication->AddLayer(layer);
}

void AppClose() {
    GApplication->Close();
}

const char *AppGetName() {
    return GApplication->GetName();
}

const TRef<IWindow> AppGetWindow() {
    return GApplication->GetWindow();
}

FRenderer &AppGetRenderer() {
    return GApplication->GetRenderer();
}

const TRef<FInput> AppGetInput() {
    return GApplication->GetInput();
}
