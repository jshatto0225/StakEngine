#include "Renderer.h"

#include "RHIPipeline.h"
#include "Window.h"
#include "RHI.h"
#include "Platform.h"

#include "RHITexture.h"
#include "RHIShader.h"

bool RendererInit(FRenderer *Renderer, FWindow *Win, bool RenderToOffscreenBuffer) {
    Renderer->Window = Win;
    Renderer->UseOffscreenBuffer = RenderToOffscreenBuffer;

    RHISetActiveViewport(Renderer->Window->Viewport);
    Renderer->SwapchainBackbuffer = Renderer->Window->Viewport->CurrentBackbuffer;
    
    Renderer->CommandContext = RHICreateCommandContext();
    if (!Renderer->CommandContext->Init()) {
        SK_LOG_ERROR("Failed to initialize command context");
        return false;
    }

    if (!Renderer->UseOffscreenBuffer) {
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
        Renderer->PipelineLayout = RHICreatePipelineLayout();
        if (!Renderer->PipelineLayout->Init(&PipelineLayoutDescription)) {
            SK_LOG_ERROR("Failed to initialize pipeline layout");
            return false;
        }

        FRHIGraphicsPipelineStateDescription PipelineDescription = {};
        PipelineDescription.ColorFormats = { Renderer->SwapchainBackbuffer->Format };
        PipelineDescription.DepthStencilFormat = { ERHIFormat::UNDEFINED };
        PipelineDescription.Layout = Renderer->PipelineLayout;
        PipelineDescription.Shaders = { VertexShader, FragmentShader };
        PipelineDescription.VertexInputAttributes = {};
        PipelineDescription.VertexInputBindings = {};

        Renderer->Pipeline = RHICreatePipeline();
        if (!Renderer->Pipeline->Init(&PipelineDescription)) {
            SK_LOG_ERROR("Failed to initialize pipeline");
            return false;
        }

        VertexShader->Shutdown();
        FragmentShader->Shutdown();
    }

    return true;
}

void RendererShutdown(FRenderer *Renderer) {
    Renderer->PipelineLayout->Shutdown();
    Renderer->Pipeline->Shutdown();

    RHIWaitForGPUIdle();
    Renderer->CommandContext->Shutdown();
}

bool RendererRender(FRenderer *Renderer) {
    RHIPrepareFrame();

    // NOTE: There will be a new backbuffer every frame
    Renderer->SwapchainBackbuffer = Renderer->Window->Viewport->CurrentBackbuffer;

    if (!Renderer->CommandContext->Begin()) {
        SK_LOG_ERROR("Failed to begin command context");
        return false;
    }
    {

        FRHIResourceBarrier RenderTargetBarrier = {};
        RenderTargetBarrier.Type = ERHIBarrierType::TRANSITION;
        RenderTargetBarrier.TransitionBarrier.Resource = Renderer->SwapchainBackbuffer;
        RenderTargetBarrier.TransitionBarrier.StateBefore = ERHIResourceState::UNDEFINED;
        RenderTargetBarrier.TransitionBarrier.StateAfter = ERHIResourceState::RENDER_TARGET;
        RenderTargetBarrier.TransitionBarrier.Subresource = 0;

        Renderer->CommandContext->ResourceBarrier(&RenderTargetBarrier);

        // NOTE: Use offscreen backbuffer if requested by the config, else use swapchain backbuffer
        if (Renderer->UseOffscreenBuffer) {
            FRHIResourceBarrier OffscreenBufferRenderTargetBarrier = {};
            OffscreenBufferRenderTargetBarrier.Type = ERHIBarrierType::TRANSITION;
            OffscreenBufferRenderTargetBarrier.TransitionBarrier.Resource = Renderer->OffscreenBackbuffers[Renderer->OffscreenBackbufferImageIndex];
            OffscreenBufferRenderTargetBarrier.TransitionBarrier.StateBefore = ERHIResourceState::UNDEFINED;
            OffscreenBufferRenderTargetBarrier.TransitionBarrier.StateAfter = ERHIResourceState::RENDER_TARGET;
            OffscreenBufferRenderTargetBarrier.TransitionBarrier.Subresource = 0;

            Renderer->CommandContext->ResourceBarrier(&OffscreenBufferRenderTargetBarrier);


            auto RenderArea = Renderer->OffscreenBackbuffers[Renderer->OffscreenBackbufferImageIndex]->RenderArea;
            Renderer->CommandContext->SetRenderTarget(Renderer->OffscreenBackbuffers[Renderer->OffscreenBackbufferImageIndex], &RenderArea);
        } else {
            auto RenderArea = Renderer->SwapchainBackbuffer->RenderArea;
            Renderer->CommandContext->SetRenderTarget(Renderer->SwapchainBackbuffer, &RenderArea);
        }

        Renderer->CommandContext->BindPipeline(Renderer->Pipeline);
        auto [LayerCount, X, Y, Width, Height] = Renderer->SwapchainBackbuffer->RenderArea;
        Renderer->CommandContext->SetViewport(static_cast<FFloat>(X), static_cast<FFloat>(Y), static_cast<FFloat>(Width), static_cast<FFloat>(Height), 0.0f, 1.0f);
        Renderer->CommandContext->SetScissor(static_cast<FSInt32>(X), static_cast<FSInt32>(Y), Width, Height);
        Renderer->CommandContext->DrawInstanced(3, 1, 0, 0);
        
        if (Renderer->UseOffscreenBuffer) {
            // NOTE: Unset offscreen backbuffer
            Renderer->CommandContext->UnsetRenderTarget();

            FRHIResourceBarrier OffscreenBufferShaderResourceBarrier = {};
            OffscreenBufferShaderResourceBarrier.Type = ERHIBarrierType::TRANSITION;
            OffscreenBufferShaderResourceBarrier.TransitionBarrier.Resource = Renderer->OffscreenBackbuffers[Renderer->OffscreenBackbufferImageIndex];
            OffscreenBufferShaderResourceBarrier.TransitionBarrier.StateBefore = ERHIResourceState::RENDER_TARGET;
            OffscreenBufferShaderResourceBarrier.TransitionBarrier.StateAfter = ERHIResourceState::SHADER_RESOURCE;
            OffscreenBufferShaderResourceBarrier.TransitionBarrier.Subresource = 0;

            Renderer->CommandContext->ResourceBarrier(&OffscreenBufferShaderResourceBarrier);

            auto RenderArea = Renderer->OffscreenBackbuffers[Renderer->OffscreenBackbufferImageIndex]->RenderArea;
            Renderer->CommandContext->SetRenderTarget(Renderer->SwapchainBackbuffer, &RenderArea);
        }

        if (Renderer->GuiRenderFunc) Renderer->GuiRenderFunc(Renderer->CommandContext);

        Renderer->CommandContext->UnsetRenderTarget();

        FRHIResourceBarrier PresentBarrier = {};
        PresentBarrier.Type = ERHIBarrierType::TRANSITION;
        PresentBarrier.TransitionBarrier.Resource = Renderer->SwapchainBackbuffer;
        PresentBarrier.TransitionBarrier.StateBefore = ERHIResourceState::RENDER_TARGET;
        PresentBarrier.TransitionBarrier.StateAfter = ERHIResourceState::PRESENT;
        PresentBarrier.TransitionBarrier.Subresource = 0;

        Renderer->CommandContext->ResourceBarrier(&PresentBarrier);
    }
    if (!Renderer->CommandContext->End()) {
        SK_LOG_ERROR("Failed to end command context");
        return false;
    }
    if (!RHISubmit(Renderer->CommandContext)) {
        SK_LOG_ERROR("Failed to submit commands");
        return false;
    }
    if (!RHIPresentFrame()) {
        SK_LOG_ERROR("Failed to present");
        return false;
    }

    return true;
}

bool RendererInitImGui(FRenderer *Renderer) {
    PlatformInitImGui(Renderer->Window);
    RHIInitImGui();

    if (Renderer->UseOffscreenBuffer) {
        FRHIOffscreenRenderTargetDescription OffscreenBackbufferDescription = {};
        OffscreenBackbufferDescription.Format = ERHIFormat::B8G8R8A8_SRGB;
        OffscreenBackbufferDescription.Width = Renderer->Window->FramebufferWidth;
        OffscreenBackbufferDescription.Height = Renderer->Window->FramebufferHeight;
        OffscreenBackbufferDescription.UseForImGui = true;
        for (auto &Backbuffer : Renderer->OffscreenBackbuffers) {
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
        Renderer->PipelineLayout = RHICreatePipelineLayout();
        if (!Renderer->PipelineLayout->Init(&PipelineLayoutDescription)) {
            SK_LOG_ERROR("Failed to initialize pipeline layout");
            return false;
        }

        FRHIGraphicsPipelineStateDescription PipelineDescription = {};
        PipelineDescription.ColorFormats = { Renderer->OffscreenBackbuffers[Renderer->OffscreenBackbufferImageIndex]->Format };
        PipelineDescription.DepthStencilFormat = { ERHIFormat::UNDEFINED };
        PipelineDescription.Layout = Renderer->PipelineLayout;
        PipelineDescription.Shaders = { VertexShader, FragmentShader };
        PipelineDescription.VertexInputAttributes = {};
        PipelineDescription.VertexInputBindings = {};

        Renderer->Pipeline = RHICreatePipeline();
        if (!Renderer->Pipeline->Init(&PipelineDescription)) {
            SK_LOG_ERROR("Failed to initialize pipeline");
            return false;
        }

        VertexShader->Shutdown();
        FragmentShader->Shutdown();
    }

    return true;
}

void RendererImGuiNewFrame(FRenderer *Renderer) {
    PlatformImGuiNewFrame();
    RHIImGuiNewFrame();
}

void RendererShutdownImGui(FRenderer *Renderer) {
    RHIWaitForGPUIdle();

    for (auto &Backbuffer : Renderer->OffscreenBackbuffers) {
        Backbuffer->Shutdown();
    }

    RHIShutdownImGui();
    PlatformShutdownImGui();
}

void RendererAddSceneToImGuiWindow(FRenderer *Renderer) {
    Renderer->OffscreenBackbuffers[Renderer->OffscreenBackbufferImageIndex]->AddToImGuiWindow();
}
