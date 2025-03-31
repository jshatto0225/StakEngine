#pragma once

#include "Types.h"

#include <imgui.h>

typedef FHandle FRHIResourceHandle;

enum class ERHIBackend {
    VULKAN,
};

enum class ERHIResourceState {
    UNDEFINED,
    RENDER_TARGET,
    SHADER_RESOURCE,
    PRESENT,
};

enum class ERHITransitionType {
    INVALID,
    IMAGE,
    BUFFER,
};

struct FRHITransitionBarrier {
    FRHIResourceHandle Resource;
    ERHIResourceState StateBefore;
    ERHIResourceState StateAfter;
    FUInt32 Subresource;
};

struct FRHIUAVBarrier {
    FRHIResourceHandle Resource;
};

struct FRHIAliasingBarrier {
    FRHIResourceHandle ResourceBefore;
    FRHIResourceHandle ResourceAfter;
};

enum class ERHIBarrierType {
    TRANSITION,
    //UAV,
    //ALIASING
};

enum class ERHIFormat {
    UNDEFINED,
    B8G8R8A8_SRGB,
};

struct FRHIResourceBarrier {
    ERHIBarrierType Type;

    union {
        FRHITransitionBarrier TransitionBarrier;
        FRHIUAVBarrier UAVBarrier;
        FRHIAliasingBarrier AliasingBarrier;
    };
};

enum class ERHIResourceType {
    TEXTURE,
    BUFFER,
};

struct FRHIRenderArea {
    FUInt32 LayerCount;
    FUInt32 X;
    FUInt32 Y;
    FUInt32 Width;
    FUInt32 Height;
};

struct FRHITextureDescription {
    FUInt32 Width;
    FUInt32 Height;
    ERHIFormat Format;
    bool UseForImGui;
    bool UseAsShaderResource;
};

enum class ERHIShaderType {
    VERTEX,
    FRAGMENT
};

struct FRHIShaderDescription {
    ERHIShaderType Type;
    const char *Name;
};

struct FRHIBufferElement {
    FUInt32 Location;
    FUInt32 Binding;
    ERHIFormat Format;
    FUInt32 Offset;
};

struct FRHIBufferLayout {
    FUInt32 ElementCount;
    FRHIBufferElement *Elements;
    FUInt32 Stride;
};

enum class ERHIBufferType {
    VERTEX,
    INDEX
};

struct FRHIVertexBufferDescription {
    FRHIBufferLayout Layout;
    FUInt32 ElementCount;
    void *InitialContents; // NOTE: Optional
    FUInt32 InitialContentsSize; // NOTE: Optional
    bool UseStagingBuffer;
};

struct FRHIIndexBufferDescription {
    FUInt32 Count;
    void *InitialContents;
    FUInt32 InitialContentsSize;
    bool UseStagingBuffer;
};

struct FRHIPipelineLayoutDescription {
    FUInt32 DescriptorSetLayoutCount;
    FRHIResourceHandle *DescriptorSetLayouts;
};

enum class ERHIPipelineType {
    GRAPHICS
};

enum class ERHIVertexInputRate {
    PER_VERTEX,
    PER_INSTANCE,
};

struct FRHIVertexInputBinding {
    FUInt32 Binding;
    FUInt32 Stride;
    ERHIVertexInputRate InputRate;
};

struct FRHIVertexInputAttribute {
    FUInt32 Location;
    FUInt32 Binding;
    ERHIFormat Format;
    FUInt32 Offset;
};

struct FRHIGraphicsPipelineStateDescription {
    FUInt32 ColorFormatCount;
    ERHIFormat *ColorFormats;
    ERHIFormat DepthStencilFormat;

    FUInt32 ShaderCount;
    FRHIResourceHandle *Shaders;

    FUInt32 VertexInputBindingCount;
    FRHIVertexInputBinding *VertexInputBindings;
    FUInt32 VertexInputAttributeCount;
    FRHIVertexInputAttribute *VertexInputAttributes;

    FRHIResourceHandle Layout;
};

enum class ERHIDescriptorType {
    UNIFORM_BUFFER,
    TEXTURE
};

struct FRHIDescriptor {
    FUInt32 Binding;
    ERHIDescriptorType Type;
    FUInt32 Count;
    FUInt32 ShaderStageCount;
    ERHIShaderType *Stages;
};

struct FRHIDescriptorSetLayoutDescription {
    FUInt32 DescriptorCount;
    FRHIDescriptor *Descriptors;
};

struct FRHICommandListDescription {
    bool IsSecondary;
};

struct FWindow;

struct FRHI {
    bool (*Init)();
    void (*Shutdown)();

    void (*ImGuiNewFrame)();
    void (*ShutdownImGui)();
    void (*InitImGui)(FRHIResourceHandle);

    bool (*WaitForGPUIdle)();

    bool (*PrepareFrame)(FRHIResourceHandle);
    bool (*PresentFrame)();
    bool (*Submit)(FRHIResourceHandle);

    void (*AddTextureToImGuiWindow)(FRHIResourceHandle);

    FRHIResourceHandle (*CreateTexture)(FRHITextureDescription *);
    void (*DestroyTexture)(FRHIResourceHandle *);

    FRHIResourceHandle (*CreateSwapchain)(FWindow *);
    void (*DestroySwapchain)(FRHIResourceHandle *);

    FRHIResourceHandle( *CreateCommandList)(FRHICommandListDescription *);
    void (*DestroyCommandList)(FRHIResourceHandle *);

    FRHIResourceHandle (*CreateShader)(FRHIShaderDescription *);
    void (*DestroyShader)(FRHIResourceHandle *);

    FRHIResourceHandle(*CreateVertexBuffer)(FRHIVertexBufferDescription *);
    FRHIResourceHandle(*CreateIndexBuffer)(FRHIIndexBufferDescription *);
    void (*DestroyBuffer)(FRHIResourceHandle *);

    FRHIResourceHandle (*CreatePipelineLayout)(FRHIPipelineLayoutDescription *);
    void (*DestroyPipelineLayout)(FRHIResourceHandle *);

    FRHIResourceHandle (*CreateDescriptorSetLayout)(FRHIDescriptorSetLayoutDescription *);
    void (*DestroyDescriptorSetLayout)(FRHIResourceHandle *);

    FRHIResourceHandle (*CreateGraphicsPipeline)(FRHIGraphicsPipelineStateDescription *);
    void (*DestroyPipeline)(FRHIResourceHandle *);

    void (*SetBufferData)(FRHIResourceHandle, void *, FUInt32);
    void (*MapBufferData)(FRHIResourceHandle, void **, FUInt32);
    bool (*SendBufferToGPU)(FRHIResourceHandle);

    FRHIResourceHandle (*GetCurrentSwapchainTexture)(FRHIResourceHandle);
    ERHIFormat (*GetTextureFormat)(FRHIResourceHandle);
    FRHIRenderArea (*GetTextureRenderArea)(FRHIResourceHandle);

    bool (*BeginCommandList)(FRHIResourceHandle);
    bool (*EndCommandList)(FRHIResourceHandle);

    void (*CmdRenderImGuiDrawData)(FRHIResourceHandle, ImDrawData *);
    void (*CmdResourceBarrier)(FRHIResourceHandle, FRHIResourceBarrier *);
    void (*CmdSetRenderTarget)(FRHIResourceHandle, FRHIResourceHandle, FRHIRenderArea *);
    void (*CmdUnsetRenderTarget)(FRHIResourceHandle);
    void (*CmdBindVertexBuffer)(FRHIResourceHandle, FRHIResourceHandle, FUInt32);
    void (*CmdBindIndexBuffer)(FRHIResourceHandle, FRHIResourceHandle);
    void (*CmdDrawIndexed)(FRHIResourceHandle, FUInt32, FUInt32, FUInt32, FSInt32, FUInt32);
    void (*CmdDrawInstanced)(FRHIResourceHandle, FUInt32, FUInt32, FUInt32, FUInt32);
    void (*CmdSetViewport)(FRHIResourceHandle, FFloat, FFloat, FFloat, FFloat, FFloat, FFloat);
    void (*CmdSetScissor)(FRHIResourceHandle, FSInt32, FSInt32, FUInt32, FUInt32);
    void (*CmdBindPipeline)(FRHIResourceHandle, FRHIResourceHandle);

    void (*NotifySwapchainOfResize)(FRHIResourceHandle);
};

ERHITransitionType RHIGetTransitionType(ERHIResourceState Before, ERHIResourceState After);

bool RHIInit(ERHIBackend Backend);
void RHIShutdown();

extern FRHI GRHI;
