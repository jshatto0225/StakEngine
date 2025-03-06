#include "Renderer.h"

#include "RHIPipeline.h"
#include "Window.h"
#include "RHI.h"

#include "RHITexture.h"
#include "RHIShader.h"

void FRenderer::Init(TRef<IWindow> Window) {
    this->Window = Window;

    RHISetActiveViewport(Window->GetRHIViewport());
    Backbuffer = Window->GetRHIViewport()->GetBackbuffer();
    CommandContext = RHICreateCommandContext();

    FRHIShaderDescription VertexShaderDescription = {};
    VertexShaderDescription.Name = "BasicShader.vert";
    VertexShaderDescription.Type = ERHIShaderType::VERTEX;
    TRef<IRHIShader> VertexShader = RHICreateShader(VertexShaderDescription);

    FRHIShaderDescription FragmentShaderDescription = {};
    FragmentShaderDescription.Name = "BasicShader.frag";
    FragmentShaderDescription.Type = ERHIShaderType::FRAGMENT;
    TRef<IRHIShader> FragmentShader = RHICreateShader(FragmentShaderDescription);

    FRHIPipelineLayoutDescription PipelineLyaoutDescription = {};
    PipelineLayout = RHICreatePipelineLayout(PipelineLyaoutDescription);

    FRHIGraphicsPipelineStateDescription PipelineDescription = {};
    PipelineDescription.ColorFormats = { Backbuffer->GetFormat() };
    PipelineDescription.DepthStencilFormat = { ERHIFormat::UNDEFINED };
    PipelineDescription.Layout = PipelineLayout;
    PipelineDescription.Shaders = { VertexShader, FragmentShader };
    PipelineDescription.VertexInputAttributes = {};
    PipelineDescription.VertexInputBindings = {};

    Pipeline = RHICreatePipeline(PipelineDescription);

    VertexShader->Shutdown();
    FragmentShader->Shutdown();
}

void FRenderer::Shutdown() {
    PipelineLayout->Shutdown();
    Pipeline->Shutdown();

    RHIWaitForGPUIdle();
    CommandContext->Shutdown();
    RHIShutdown();
}

void FRenderer::Render() {
    RHIPrepareFrame();

    // NOTE: Assume backbuffer will not change
    // TODO: Handle backbuffer changes

    CommandContext->Begin();
    {
        FRHIResourceBarrier RenderTargetBarrier = {
            ERHIBarrierType::TRANSITION,

            Backbuffer,
            ERHIResourceState::UNDEFINED,
            ERHIResourceState::RENDER_TARGET,
            0
        };

        CommandContext->ResourceBarrier(RenderTargetBarrier);

        CommandContext->SetRenderTarget(Backbuffer, Backbuffer->GetRenderArea());

        CommandContext->BindPipeline(Pipeline);
        auto [LayerCount, X, Y, Width, Height] = Backbuffer->GetRenderArea();
        CommandContext->SetViewport(static_cast<FFloat>(X), static_cast<FFloat>(Y), static_cast<FFloat>(Width), static_cast<FFloat>(Height), 0.0f, 1.0f);
        CommandContext->SetScissor(static_cast<FSInt32>(X), static_cast<FSInt32>(Y), Width, Height);
        CommandContext->DrawInstanced(3, 1, 0, 0);

        for (auto Proxy : RenderProxies) {
            Proxy->Render(CommandContext);
        }

        if (PostRenderProxy) {
            PostRenderProxy->Render(CommandContext);
        }

        CommandContext->UnsetRenderTarget();

        FRHIResourceBarrier PresentBarrier = {
            ERHIBarrierType::TRANSITION,

            Backbuffer,
            ERHIResourceState::RENDER_TARGET,
            ERHIResourceState::PRESENT,
            0
        };

        CommandContext->ResourceBarrier(PresentBarrier);
    }
    CommandContext->End();

    RHISubmit(CommandContext);

    RHIPresentFrame();
}

void FRenderer::InitImGui() {
    Window->InitImGui();
    RHIInitImGui();
}

void FRenderer::ImGuiNewFrame() {
    Window->ImGuiNewFrame();
    RHIImGuiNewFrame();
}

void FRenderer::ShutdownImGui() {
    RHIWaitForGPUIdle();
    RHIShutdownImGui();
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
