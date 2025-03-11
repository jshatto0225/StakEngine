#include "Renderer.h"

#include "RHIPipeline.h"
#include "Window.h"
#include "RHI.h"

#include "RHITexture.h"
#include "RHIShader.h"

bool FRenderer::Init(TRef<IWindow> Win, bool RenderToOffscreenBuffer) {
    Window = Win;
    UseOffscreenBuffer = RenderToOffscreenBuffer;

    RHISetActiveViewport(Window->GetRHIViewport());
    SwapchainBackbuffer = Window->GetRHIViewport()->GetCurrentBackbuffer();
    
    CommandContext = RHICreateCommandContext();
    if (!CommandContext->Init()) {
        SK_LOG_ERROR("Failed to initialize command context");
        return false;
    }

    if (!UseOffscreenBuffer) {
        FRHIShaderDescription VertexShaderDescription = {};
        VertexShaderDescription.Name = "BasicShader.vert";
        VertexShaderDescription.Type = ERHIShaderType::VERTEX;
        TRef<IRHIShader> VertexShader = RHICreateShader();
        if (!VertexShader->Init(&VertexShaderDescription)) {
            SK_LOG_ERROR("Failed to initialize vertex shader");
            return false;
        }

        FRHIShaderDescription FragmentShaderDescription = {};
        FragmentShaderDescription.Name = "BasicShader.frag";
        FragmentShaderDescription.Type = ERHIShaderType::FRAGMENT;
        TRef<IRHIShader> FragmentShader = RHICreateShader();
        if (!FragmentShader->Init(&FragmentShaderDescription)) {
            SK_LOG_ERROR("Failed to initialize fragment shader");
            return false;
        }

        FRHIPipelineLayoutDescription PipelineLayoutDescription = {};
        PipelineLayout = RHICreatePipelineLayout();
        if (!PipelineLayout->Init(&PipelineLayoutDescription)) {
            SK_LOG_ERROR("Failed to initialize pipeline layout");
            return false;
        }

        FRHIGraphicsPipelineStateDescription PipelineDescription = {};
        PipelineDescription.ColorFormats = { SwapchainBackbuffer->GetFormat() };
        PipelineDescription.DepthStencilFormat = { ERHIFormat::UNDEFINED };
        PipelineDescription.Layout = PipelineLayout;
        PipelineDescription.Shaders = { VertexShader, FragmentShader };
        PipelineDescription.VertexInputAttributes = {};
        PipelineDescription.VertexInputBindings = {};

        Pipeline = RHICreatePipeline();
        if (!Pipeline->Init(&PipelineDescription)) {
            SK_LOG_ERROR("Failed to initialize pipeline");
            return false;
        }

        VertexShader->Shutdown();
        FragmentShader->Shutdown();
    }

    return true;
}

void FRenderer::Shutdown() {
    PipelineLayout->Shutdown();
    Pipeline->Shutdown();

    RHIWaitForGPUIdle();
    CommandContext->Shutdown();
}

bool FRenderer::Render() {
    RHIPrepareFrame();

    // NOTE: There will be a new backbuffer every frame
    SwapchainBackbuffer = Window->GetRHIViewport()->GetCurrentBackbuffer();

    if (!CommandContext->Begin()) {
        SK_LOG_ERROR("Failed to begin command context");
        return false;
    }
    {

        FRHIResourceBarrier RenderTargetBarrier = {};
        RenderTargetBarrier.Type = ERHIBarrierType::TRANSITION;
        RenderTargetBarrier.TransitionBarrier.Resource = SwapchainBackbuffer;
        RenderTargetBarrier.TransitionBarrier.StateBefore = ERHIResourceState::UNDEFINED;
        RenderTargetBarrier.TransitionBarrier.StateAfter = ERHIResourceState::RENDER_TARGET;
        RenderTargetBarrier.TransitionBarrier.Subresource = 0;

        CommandContext->ResourceBarrier(&RenderTargetBarrier);

        // NOTE: Use offscreen backbuffer if requested by the config, else use swapchain backbuffer
        if (UseOffscreenBuffer) {
            FRHIResourceBarrier OffscreenBufferRenderTargetBarrier = {};
            OffscreenBufferRenderTargetBarrier.Type = ERHIBarrierType::TRANSITION;
            OffscreenBufferRenderTargetBarrier.TransitionBarrier.Resource = OffscreenBackbuffers[OffscreenBackbufferImageIndex];
            OffscreenBufferRenderTargetBarrier.TransitionBarrier.StateBefore = ERHIResourceState::UNDEFINED;
            OffscreenBufferRenderTargetBarrier.TransitionBarrier.StateAfter = ERHIResourceState::RENDER_TARGET;
            OffscreenBufferRenderTargetBarrier.TransitionBarrier.Subresource = 0;

            CommandContext->ResourceBarrier(&OffscreenBufferRenderTargetBarrier);


            auto RenderArea = OffscreenBackbuffers[OffscreenBackbufferImageIndex]->GetRenderArea();
            CommandContext->SetRenderTarget(OffscreenBackbuffers[OffscreenBackbufferImageIndex], &RenderArea);
        } else {
            auto RenderArea = SwapchainBackbuffer->GetRenderArea();
            CommandContext->SetRenderTarget(SwapchainBackbuffer, &RenderArea);
        }

        CommandContext->BindPipeline(Pipeline);
        auto [LayerCount, X, Y, Width, Height] = SwapchainBackbuffer->GetRenderArea();
        CommandContext->SetViewport(static_cast<FFloat>(X), static_cast<FFloat>(Y), static_cast<FFloat>(Width), static_cast<FFloat>(Height), 0.0f, 1.0f);
        CommandContext->SetScissor(static_cast<FSInt32>(X), static_cast<FSInt32>(Y), Width, Height);
        CommandContext->DrawInstanced(3, 1, 0, 0);

        for (auto Proxy : RenderProxies) {
            Proxy->Render(CommandContext);
        }
        
        if (UseOffscreenBuffer) {
            // NOTE: Unset offscreen backbuffer
            CommandContext->UnsetRenderTarget();

            FRHIResourceBarrier OffscreenBufferShaderResourceBarrier = {};
            OffscreenBufferShaderResourceBarrier.Type = ERHIBarrierType::TRANSITION;
            OffscreenBufferShaderResourceBarrier.TransitionBarrier.Resource = OffscreenBackbuffers[OffscreenBackbufferImageIndex];
            OffscreenBufferShaderResourceBarrier.TransitionBarrier.StateBefore = ERHIResourceState::RENDER_TARGET;
            OffscreenBufferShaderResourceBarrier.TransitionBarrier.StateAfter = ERHIResourceState::SHADER_RESOURCE;
            OffscreenBufferShaderResourceBarrier.TransitionBarrier.Subresource = 0;

            CommandContext->ResourceBarrier(&OffscreenBufferShaderResourceBarrier);

            auto RenderArea = OffscreenBackbuffers[OffscreenBackbufferImageIndex]->GetRenderArea();
            CommandContext->SetRenderTarget(SwapchainBackbuffer, &RenderArea);
        }
        
        if (PostRenderProxy) {
            PostRenderProxy->Render(CommandContext);
        }

        CommandContext->UnsetRenderTarget();

        FRHIResourceBarrier PresentBarrier = {};
        PresentBarrier.Type = ERHIBarrierType::TRANSITION;
        PresentBarrier.TransitionBarrier.Resource = SwapchainBackbuffer;
        PresentBarrier.TransitionBarrier.StateBefore = ERHIResourceState::RENDER_TARGET;
        PresentBarrier.TransitionBarrier.StateAfter = ERHIResourceState::PRESENT;
        PresentBarrier.TransitionBarrier.Subresource = 0;

        CommandContext->ResourceBarrier(&PresentBarrier);
    }
    if (!CommandContext->End()) {
        SK_LOG_ERROR("Failed to end command context");
        return false;
    }
    if (!RHISubmit(CommandContext)) {
        SK_LOG_ERROR("Failed to submit commands");
        return false;
    }
    if (!RHIPresentFrame()) {
        SK_LOG_ERROR("Failed to present");
        return false;
    }

    return true;
}

bool FRenderer::InitImGui() {
    Window->InitImGui();
    RHIInitImGui();

    if (UseOffscreenBuffer) {
        auto [Width, Height] = Window->GetFramebufferSize();
        FRHIOffscreenRenderTargetDescription OffscreenBackbufferDescription = {};
        OffscreenBackbufferDescription.Format = ERHIFormat::B8G8R8A8_SRGB;
        OffscreenBackbufferDescription.Width = Width;
        OffscreenBackbufferDescription.Height = Height;
        OffscreenBackbufferDescription.UseForImGui = true;
        for (auto &Backbuffer : OffscreenBackbuffers) {
            Backbuffer = RHICreateTexture();
            if (!Backbuffer->Init(&OffscreenBackbufferDescription)) {
                SK_LOG_ERROR("Failed to initialize offscreen backbuffer");
                return false;
            }
        }

        FRHIShaderDescription VertexShaderDescription = {};
        VertexShaderDescription.Name = "BasicShader.vert";
        VertexShaderDescription.Type = ERHIShaderType::VERTEX;
        TRef<IRHIShader> VertexShader = RHICreateShader();
        if (!VertexShader->Init(&VertexShaderDescription)) {
            SK_LOG_ERROR("Failed to initialize vertex shader");
            return false;
        }

        FRHIShaderDescription FragmentShaderDescription = {};
        FragmentShaderDescription.Name = "BasicShader.frag";
        FragmentShaderDescription.Type = ERHIShaderType::FRAGMENT;
        TRef<IRHIShader> FragmentShader = RHICreateShader();
        if (!FragmentShader->Init(&FragmentShaderDescription)) {
            SK_LOG_ERROR("Failed to initialize fragment shader");
            return false;
        }

        FRHIPipelineLayoutDescription PipelineLayoutDescription = {};
        PipelineLayout = RHICreatePipelineLayout();
        if (!PipelineLayout->Init(&PipelineLayoutDescription)) {
            SK_LOG_ERROR("Failed to initialize pipeline layout");
            return false;
        }

        FRHIGraphicsPipelineStateDescription PipelineDescription = {};
        PipelineDescription.ColorFormats = { OffscreenBackbuffers[OffscreenBackbufferImageIndex]->GetFormat()};
        PipelineDescription.DepthStencilFormat = { ERHIFormat::UNDEFINED };
        PipelineDescription.Layout = PipelineLayout;
        PipelineDescription.Shaders = { VertexShader, FragmentShader };
        PipelineDescription.VertexInputAttributes = {};
        PipelineDescription.VertexInputBindings = {};

        Pipeline = RHICreatePipeline();
        if (!Pipeline->Init(&PipelineDescription)) {
            SK_LOG_ERROR("Failed to initialize pipeline");
            return false;
        }

        VertexShader->Shutdown();
        FragmentShader->Shutdown();
    }

    return true;
}

void FRenderer::ImGuiNewFrame() {
    Window->ImGuiNewFrame();
    RHIImGuiNewFrame();
}

void FRenderer::ShutdownImGui() {
    RHIWaitForGPUIdle();

    for (auto &Backbuffer : OffscreenBackbuffers) {
        Backbuffer->Shutdown();
    }

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

void FRenderer::AddSceneToImGuiWindow() {
    OffscreenBackbuffers[OffscreenBackbufferImageIndex]->AddToImGuiWindow();
}
