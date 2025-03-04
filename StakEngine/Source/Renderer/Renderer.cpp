#include "Renderer.h"

#include "RHIPipeline.h"
#include "Window.h"
#include "RHI.h"

#include "RHITexture.h"
#include "RHIShader.h"

void FRenderer::Init(TRef<IWindow> Window) {
    this->Window = Window;
    Device = RHICreateDevice(Window);
    CommandContext = Device.CreateCommandContext();

    Backbuffer = Device.GetBackbuffer();

    FRHIShaderDescription VertexShaderDescription = {};
    VertexShaderDescription.Name = "BasicShader.vert";
    VertexShaderDescription.Type = ERHIShaderType::VERTEX;
    FRHIShader VertexShader = Device.CreateShader(VertexShaderDescription);

    FRHIShaderDescription FragmentShaderDescription = {};
    FragmentShaderDescription.Name = "BasicShader.frag";
    FragmentShaderDescription.Type = ERHIShaderType::FRAGMENT;
    FRHIShader FragmentShader = Device.CreateShader(FragmentShaderDescription);

    FRHIPipelineLayoutDescription PipelineLyaoutDescription = {};
    PipelineLayout = Device.CreatePipelineLayout(PipelineLyaoutDescription);

    FRHIGraphicsPipelineStateDescription PipelineDescription = {};
    PipelineDescription.ColorFormats = { Backbuffer.GetFormat() };
    PipelineDescription.DepthStencilFormat = { ERHIFormat::UNDEFINED };
    PipelineDescription.Layout = &PipelineLayout;
    PipelineDescription.Shaders = { &VertexShader, &FragmentShader };
    PipelineDescription.VertexInputAttributes = {};
    PipelineDescription.VertexInputBindings = {};

    Pipeline = Device.CreatePipeline(PipelineDescription);

    VertexShader.Shutdown();
    FragmentShader.Shutdown();
}

void FRenderer::Shutdown() {
    PipelineLayout.Shutdown();
    Pipeline.Shutdown();

    Device.WaitForGPUIdle();
    CommandContext.Shutdown();
    Device.Shutdown();
}

void FRenderer::Render() {
    Device.PrepareFrame();

    CommandContext.Begin();
    {
        FRHIResourceBarrier RenderTargetBarrier = {
            ERHIBarrierType::TRANSITION,

            Backbuffer,
            ERHIResourceState::UNDEFINED,
            ERHIResourceState::RENDER_TARGET,
            0
        };

        CommandContext.ResourceBarrier(RenderTargetBarrier);

        CommandContext.SetRenderTarget(Backbuffer, Backbuffer.GetRenderArea());

        CommandContext.BindPipeline(Pipeline);

        auto [LayerCount, X, Y, Width, Height] = Backbuffer.GetRenderArea();

        CommandContext.SetViewport(static_cast<FFloat>(X), static_cast<FFloat>(Y), Width, Height, 0.0f, 1.0f);
        CommandContext.SetScissor(static_cast<FSInt32>(X), static_cast<FSInt32>(Y), Width, Height);

        CommandContext.DrawInstanced(3, 1, 0, 0);

        for (auto Proxy : RenderProxies) {
            Proxy->Render(CommandContext);
        }

        if (PostRenderProxy) {
            PostRenderProxy->Render(CommandContext);
        }

        CommandContext.UnsetRenderTarget();

        FRHIResourceBarrier PresentBarrier = {
            ERHIBarrierType::TRANSITION,

            Backbuffer,
            ERHIResourceState::RENDER_TARGET,
            ERHIResourceState::PRESENT,
            0
        };

        CommandContext.ResourceBarrier(PresentBarrier);
    }
    CommandContext.End();

    Device.Submit(CommandContext);

    Device.PresentFrame();
}

void FRenderer::InitImGui() {
    Window->InitImGui();
    Device.InitImGui();
}

void FRenderer::ImGuiNewFrame() {
    Window->ImGuiNewFrame();
    Device.ImGuiNewFrame();
}

void FRenderer::ShutdownImGui() {
    Device.WaitForGPUIdle();
    Device.ShutdownImGui();
    Window->ShutdownImGui();
}

void FRenderer::AddProxy(FRenderProxy *Proxy) {
    RenderProxies.push_back(Proxy);
}

void FRenderer::RemoveProxy(FRenderProxy *Proxy) {
    auto It = std::find(RenderProxies.begin(), RenderProxies.end(), Proxy);
    if (It != RenderProxies.end()) {
        RenderProxies.erase(It);
    }
}

void FRenderer::SetPostProxy(FRenderProxy *Proxy) {
    PostRenderProxy = Proxy;
}

void FRenderer::UnsetPostProxy() {
    PostRenderProxy = nullptr;
}
