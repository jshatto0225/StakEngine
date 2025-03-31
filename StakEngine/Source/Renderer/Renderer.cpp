#include "Renderer.h"

#include "Window.h"
#include "RHI.h"
#include "Platform.h"
#include "Log.h"

bool RendererInit(FRenderer *Renderer, FWindow *Win, bool RenderToOffscreenBuffer) {
    Renderer->Window = Win;
    Renderer->UseOffscreenBuffer = RenderToOffscreenBuffer;

    Renderer->SwapchainTexture = GRHI.GetCurrentSwapchainTexture(Renderer->Window->Swapchain);

    FRHICommandListDescription CommandListDescription = {};
    CommandListDescription.IsSecondary = false;
    Renderer->CommandList = GRHI.CreateCommandList(&CommandListDescription);
    if (!Renderer->CommandList) {
        SK_LOG_ERROR("Failed to initialize command context");
        return false;
    }

    if (!Renderer->UseOffscreenBuffer) {
        FRHIShaderDescription VertexShaderDescription = {};
        VertexShaderDescription.Name = "BasicShader.vert";
        VertexShaderDescription.Type = ERHIShaderType::VERTEX;
        FRHIResourceHandle VertexShader = GRHI.CreateShader(&VertexShaderDescription);
        if (!VertexShader) {
            SK_LOG_ERROR("Failed to initialize vertex shader");
            return false;
        }

        FRHIShaderDescription FragmentShaderDescription = {};
        FragmentShaderDescription.Name = "BasicShader.frag";
        FragmentShaderDescription.Type = ERHIShaderType::FRAGMENT;
        FRHIResourceHandle FragmentShader = GRHI.CreateShader(&FragmentShaderDescription);
        if (!FragmentShader) {
            SK_LOG_ERROR("Failed to initialize fragment shader");
            return false;
        }

        FRHIPipelineLayoutDescription PipelineLayoutDescription = {};
        Renderer->PipelineLayout = GRHI.CreatePipelineLayout(&PipelineLayoutDescription);
        if (!Renderer->PipelineLayout) {
            SK_LOG_ERROR("Failed to initialize pipeline layout");
            return false;
        }

        FRHIResourceHandle Shaders[] = { VertexShader, FragmentShader };
        ERHIFormat Formats[] = { GRHI.GetTextureFormat(Renderer->SwapchainTexture) };

        FRHIGraphicsPipelineStateDescription PipelineDescription = {};
        PipelineDescription.ColorFormats = Formats;
        PipelineDescription.DepthStencilFormat = { ERHIFormat::UNDEFINED };
        PipelineDescription.Layout = Renderer->PipelineLayout;
        PipelineDescription.ShaderCount = 2;
        PipelineDescription.Shaders = Shaders;
        PipelineDescription.VertexInputAttributes = {};
        PipelineDescription.VertexInputBindings = {};

        Renderer->Pipeline = GRHI.CreateGraphicsPipeline(&PipelineDescription);
        if (!Renderer->Pipeline) {
            SK_LOG_ERROR("Failed to initialize pipeline");
            return false;
        }

        GRHI.DestroyShader(&VertexShader);
        GRHI.DestroyShader(&FragmentShader);
    }

    return true;
}

void RendererShutdown(FRenderer *Renderer) {
    GRHI.DestroyPipelineLayout(&Renderer->PipelineLayout);
    GRHI.DestroyPipeline(&Renderer->Pipeline);

    GRHI.WaitForGPUIdle();
    GRHI.DestroyCommandList(&Renderer->CommandList);
}

bool RendererRender(FRenderer *Renderer) {
    GRHI.PrepareFrame(Renderer->Window->Swapchain);

    // NOTE: There will be a new backbuffer every frame
    Renderer->SwapchainTexture = GRHI.GetCurrentSwapchainTexture(Renderer->Window->Swapchain);

    if (!GRHI.BeginCommandList(Renderer->CommandList)) {
        SK_LOG_ERROR("Failed to begin command context");
        return false;
    }
    {

        FRHIResourceBarrier RenderTargetBarrier = {};
        RenderTargetBarrier.Type = ERHIBarrierType::TRANSITION;
        RenderTargetBarrier.TransitionBarrier.Resource = Renderer->SwapchainTexture;
        RenderTargetBarrier.TransitionBarrier.StateBefore = ERHIResourceState::UNDEFINED;
        RenderTargetBarrier.TransitionBarrier.StateAfter = ERHIResourceState::RENDER_TARGET;
        RenderTargetBarrier.TransitionBarrier.Subresource = 0;

        GRHI.CmdResourceBarrier(Renderer->CommandList, &RenderTargetBarrier);

        // NOTE: Use offscreen backbuffer if requested by the config, else use swapchain backbuffer
        if (Renderer->UseOffscreenBuffer) {
            FRHIResourceBarrier OffscreenBufferRenderTargetBarrier = {};
            OffscreenBufferRenderTargetBarrier.Type = ERHIBarrierType::TRANSITION;
            OffscreenBufferRenderTargetBarrier.TransitionBarrier.Resource = Renderer->OffscreenBackbuffers[Renderer->OffscreenBackbufferImageIndex];
            OffscreenBufferRenderTargetBarrier.TransitionBarrier.StateBefore = ERHIResourceState::UNDEFINED;
            OffscreenBufferRenderTargetBarrier.TransitionBarrier.StateAfter = ERHIResourceState::RENDER_TARGET;
            OffscreenBufferRenderTargetBarrier.TransitionBarrier.Subresource = 0;

            GRHI.CmdResourceBarrier(Renderer->CommandList, &OffscreenBufferRenderTargetBarrier);


            auto RenderArea = GRHI.GetTextureRenderArea(Renderer->OffscreenBackbuffers[Renderer->OffscreenBackbufferImageIndex]);
            GRHI.CmdSetRenderTarget(Renderer->CommandList, Renderer->OffscreenBackbuffers[Renderer->OffscreenBackbufferImageIndex], &RenderArea);
        } else {
            auto RenderArea = GRHI.GetTextureRenderArea(Renderer->SwapchainTexture);
            GRHI.CmdSetRenderTarget(Renderer->CommandList, Renderer->SwapchainTexture, &RenderArea);
        }

        GRHI.CmdBindPipeline(Renderer->CommandList, Renderer->Pipeline);
        auto [LayerCount, X, Y, Width, Height] = GRHI.GetTextureRenderArea(Renderer->SwapchainTexture);
        GRHI.CmdSetViewport(Renderer->CommandList, (FFloat)X, (FFloat)Y, (FFloat)Width, (FFloat)Height, 0.0f, 1.0f);
        GRHI.CmdSetScissor(Renderer->CommandList, (FSInt32)X, (FSInt32)Y, Width, Height);
        GRHI.CmdDrawInstanced(Renderer->CommandList, 3, 1, 0, 0);

        if (Renderer->UseOffscreenBuffer) {
            // NOTE: Unset offscreen backbuffer
            GRHI.CmdUnsetRenderTarget(Renderer->CommandList);

            FRHIResourceBarrier OffscreenBufferShaderResourceBarrier = {};
            OffscreenBufferShaderResourceBarrier.Type = ERHIBarrierType::TRANSITION;
            OffscreenBufferShaderResourceBarrier.TransitionBarrier.Resource = Renderer->OffscreenBackbuffers[Renderer->OffscreenBackbufferImageIndex];
            OffscreenBufferShaderResourceBarrier.TransitionBarrier.StateBefore = ERHIResourceState::RENDER_TARGET;
            OffscreenBufferShaderResourceBarrier.TransitionBarrier.StateAfter = ERHIResourceState::SHADER_RESOURCE;
            OffscreenBufferShaderResourceBarrier.TransitionBarrier.Subresource = 0;

            GRHI.CmdResourceBarrier(Renderer->CommandList, &OffscreenBufferShaderResourceBarrier);

            auto RenderArea = GRHI.GetTextureRenderArea(Renderer->OffscreenBackbuffers[Renderer->OffscreenBackbufferImageIndex]);
            GRHI.CmdSetRenderTarget(Renderer->CommandList, Renderer->SwapchainTexture, &RenderArea);
        }

        if (Renderer->GuiRenderFunc) Renderer->GuiRenderFunc(Renderer->CommandList);

        GRHI.CmdUnsetRenderTarget(Renderer->CommandList);

        FRHIResourceBarrier PresentBarrier = {};
        PresentBarrier.Type = ERHIBarrierType::TRANSITION;
        PresentBarrier.TransitionBarrier.Resource = Renderer->SwapchainTexture;
        PresentBarrier.TransitionBarrier.StateBefore = ERHIResourceState::RENDER_TARGET;
        PresentBarrier.TransitionBarrier.StateAfter = ERHIResourceState::PRESENT;
        PresentBarrier.TransitionBarrier.Subresource = 0;

        GRHI.CmdResourceBarrier(Renderer->CommandList, &PresentBarrier);
    }
    if (!GRHI.EndCommandList(Renderer->CommandList)) {
        SK_LOG_ERROR("Failed to end command context");
        return false;
    }
    if (!GRHI.Submit(Renderer->CommandList)) {
        SK_LOG_ERROR("Failed to submit commands");
        return false;
    }
    if (!GRHI.PresentFrame()) {
        SK_LOG_ERROR("Failed to present");
        return false;
    }

    return true;
}

bool RendererInitImGui(FRenderer *Renderer) {
    PlatformInitImGui(Renderer->Window);
    // NOTE: Assume all swapchain textures have the same format
    GRHI.InitImGui(Renderer->Window->Swapchain);

    if (Renderer->UseOffscreenBuffer) {
        FRHITextureDescription OffscreenBackbufferDescription = {};
        OffscreenBackbufferDescription.Format = ERHIFormat::B8G8R8A8_SRGB;
        OffscreenBackbufferDescription.Width = Renderer->Window->FramebufferWidth;
        OffscreenBackbufferDescription.Height = Renderer->Window->FramebufferHeight;
        OffscreenBackbufferDescription.UseForImGui = true;
        OffscreenBackbufferDescription.UseAsShaderResource = true;
        for (auto &Backbuffer : Renderer->OffscreenBackbuffers) {
            Backbuffer = GRHI.CreateTexture(&OffscreenBackbufferDescription);
            if (!Backbuffer) {
                SK_LOG_ERROR("Failed to initialize offscreen backbuffer");
                return false;
            }
        }

        FRHIShaderDescription VertexShaderDescription = {};
        VertexShaderDescription.Name = "BasicShader.vert";
        VertexShaderDescription.Type = ERHIShaderType::VERTEX;
        FRHIResourceHandle VertexShader = GRHI.CreateShader(&VertexShaderDescription);
        if (!VertexShader) {
            SK_LOG_ERROR("Failed to initialize vertex shader");
            return false;
        }

        FRHIShaderDescription FragmentShaderDescription = {};
        FragmentShaderDescription.Name = "BasicShader.frag";
        FragmentShaderDescription.Type = ERHIShaderType::FRAGMENT;
        FRHIResourceHandle FragmentShader = GRHI.CreateShader(&FragmentShaderDescription);
        if (!FragmentShader) {
            SK_LOG_ERROR("Failed to initialize fragment shader");
            return false;
        }

        FRHIPipelineLayoutDescription PipelineLayoutDescription = {};
        Renderer->PipelineLayout = GRHI.CreatePipelineLayout(&PipelineLayoutDescription);
        if (!Renderer->PipelineLayout) {
            SK_LOG_ERROR("Failed to initialize pipeline layout");
            return false;
        }

        ERHIFormat Formats[] = { ERHIFormat::B8G8R8A8_SRGB };
        FRHIResourceHandle Shaders[] = { VertexShader, FragmentShader };

        FRHIGraphicsPipelineStateDescription PipelineDescription = {};
        PipelineDescription.ColorFormats = Formats;
        PipelineDescription.DepthStencilFormat = { ERHIFormat::UNDEFINED };
        PipelineDescription.Layout = Renderer->PipelineLayout;
        PipelineDescription.ShaderCount = 2;
        PipelineDescription.Shaders = Shaders;
        PipelineDescription.VertexInputAttributes = {};
        PipelineDescription.VertexInputBindings = {};

        Renderer->Pipeline = GRHI.CreateGraphicsPipeline(&PipelineDescription);
        if (!Renderer->Pipeline) {
            SK_LOG_ERROR("Failed to initialize pipeline");
            return false;
        }

        GRHI.DestroyShader(&VertexShader);
        GRHI.DestroyShader(&FragmentShader);
    }

    return true;
}

void RendererImGuiNewFrame(FRenderer *Renderer) {
    PlatformImGuiNewFrame();
    GRHI.ImGuiNewFrame();
}

void RendererShutdownImGui(FRenderer *Renderer) {
    GRHI.WaitForGPUIdle();

    for (auto &Backbuffer : Renderer->OffscreenBackbuffers) {
        GRHI.DestroyTexture(&Backbuffer);
    }

    GRHI.ShutdownImGui();
    PlatformShutdownImGui();
}

void RendererAddSceneToImGuiWindow(FRenderer *Renderer) {
    GRHI.AddTextureToImGuiWindow(Renderer->OffscreenBackbuffers[Renderer->OffscreenBackbufferImageIndex]);
}
