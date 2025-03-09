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
    SwapchainBackbuffer = Window->GetRHIViewport()->GetBackbuffer();
    
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
        if (!VertexShader->Init(VertexShaderDescription)) {
            SK_LOG_ERROR("Failed to initialize vertex shader");
            return false;
        }

        FRHIShaderDescription FragmentShaderDescription = {};
        FragmentShaderDescription.Name = "BasicShader.frag";
        FragmentShaderDescription.Type = ERHIShaderType::FRAGMENT;
        TRef<IRHIShader> FragmentShader = RHICreateShader();
        if (!FragmentShader->Init(FragmentShaderDescription)) {
            SK_LOG_ERROR("Failed to initialize fragment shader");
            return false;
        }

        FRHIPipelineLayoutDescription PipelineLayoutDescription = {};
        PipelineLayout = RHICreatePipelineLayout();
        if (!PipelineLayout->Init(PipelineLayoutDescription)) {
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
        if (!Pipeline->Init(PipelineDescription)) {
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
    RHIShutdown();
}

bool FRenderer::Render() {
    RHIPrepareFrame();

    // NOTE: Assume swapchain backbuffer will not change
    // TODO: Handle swapchain backbuffer changes

    if (!CommandContext->Begin()) {
        SK_LOG_ERROR("Failed to begin command context");
        return false;
    }
    {
        FRHIResourceBarrier RenderTargetBarrier = {
            ERHIBarrierType::TRANSITION,

            SwapchainBackbuffer,
            ERHIResourceState::UNDEFINED,
            ERHIResourceState::RENDER_TARGET,
            0
        };

        CommandContext->ResourceBarrier(RenderTargetBarrier);

        // NOTE: Use offscreen backbuffer if requested by the config, else use swapchain backbuffer
        if (UseOffscreenBuffer) {
            FRHIResourceBarrier OffscreenBufferRenderTargetBarrier = {
                ERHIBarrierType::TRANSITION,

                OffscreenBackbuffer,
                ERHIResourceState::UNDEFINED,
                ERHIResourceState::RENDER_TARGET,
                0
            };

            CommandContext->ResourceBarrier(OffscreenBufferRenderTargetBarrier);

            CommandContext->SetRenderTarget(OffscreenBackbuffer, OffscreenBackbuffer->GetRenderArea());
        } else {
            CommandContext->SetRenderTarget(SwapchainBackbuffer, SwapchainBackbuffer->GetRenderArea());
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

            FRHIResourceBarrier OffscreenBufferRenderTargetBarrier = {
                ERHIBarrierType::TRANSITION,

                OffscreenBackbuffer,
                ERHIResourceState::RENDER_TARGET,
                ERHIResourceState::SHADER_RESOURCE,
                0
            };

            CommandContext->ResourceBarrier(OffscreenBufferRenderTargetBarrier);

            // NOTE: Setup swapchain for imgui 
            CommandContext->SetRenderTarget(SwapchainBackbuffer, SwapchainBackbuffer->GetRenderArea());
        }
        
        if (PostRenderProxy) {
            PostRenderProxy->Render(CommandContext);
        }

        CommandContext->UnsetRenderTarget();

        FRHIResourceBarrier PresentBarrier = {
            ERHIBarrierType::TRANSITION,

            SwapchainBackbuffer,
            ERHIResourceState::RENDER_TARGET,
            ERHIResourceState::PRESENT,
            0
        };

        CommandContext->ResourceBarrier(PresentBarrier);
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
        OffscreenBackbuffer = RHICreateTexture();
        if (!OffscreenBackbuffer->Init(OffscreenBackbufferDescription)) {
            SK_LOG_ERROR("Failed to initialize offscreen backbuffer");
            return false;
        }

        FRHIShaderDescription VertexShaderDescription = {};
        VertexShaderDescription.Name = "BasicShader.vert";
        VertexShaderDescription.Type = ERHIShaderType::VERTEX;
        TRef<IRHIShader> VertexShader = RHICreateShader();
        if (!VertexShader->Init(VertexShaderDescription)) {
            SK_LOG_ERROR("Failed to initialize vertex shader");
            return false;
        }

        FRHIShaderDescription FragmentShaderDescription = {};
        FragmentShaderDescription.Name = "BasicShader.frag";
        FragmentShaderDescription.Type = ERHIShaderType::FRAGMENT;
        TRef<IRHIShader> FragmentShader = RHICreateShader();
        if (!FragmentShader->Init(FragmentShaderDescription)) {
            SK_LOG_ERROR("Failed to initialize fragment shader");
            return false;
        }

        FRHIPipelineLayoutDescription PipelineLayoutDescription = {};
        PipelineLayout = RHICreatePipelineLayout();
        if (!PipelineLayout->Init(PipelineLayoutDescription)) {
            SK_LOG_ERROR("Failed to initialize pipeline layout");
            return false;
        }

        FRHIGraphicsPipelineStateDescription PipelineDescription = {};
        PipelineDescription.ColorFormats = { OffscreenBackbuffer->GetFormat() };
        PipelineDescription.DepthStencilFormat = { ERHIFormat::UNDEFINED };
        PipelineDescription.Layout = PipelineLayout;
        PipelineDescription.Shaders = { VertexShader, FragmentShader };
        PipelineDescription.VertexInputAttributes = {};
        PipelineDescription.VertexInputBindings = {};

        Pipeline = RHICreatePipeline();
        if (!Pipeline->Init(PipelineDescription)) {
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
    RHIAddBackbufferToImGuiWindow(OffscreenBackbuffer);
}
