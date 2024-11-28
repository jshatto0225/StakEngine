#pragma once

#include "Types.h"
#include "RHIForward.h"

#include <imgui.h>

enum class ERHIAccess {

};

enum class ERHIUniformBufferUsage {

};

enum class EBufferUsageFlags {

};

enum class ERHITransitionCreateFlags {

};

struct FRHIUniformBufferLayout {

};

struct FRHIBufferDescription {

};

struct FRHISamplerStateInitializer {

};

struct FRHIRasterizerStateInitializer {

};

struct FRHIDepthStencilStateInitializer {

};

struct FRHIBlendStateInitializer {

};

struct FRHIVertexDeclarationElementList {

};

struct FRHIGraphicsPipelineStateInitializer {

};

struct FRHITextureDescription {

};

struct FRHIViewDescription {

};

struct FRHIResourceCreateInfo {

};

class FRHI {
public:
  FRHI();
  virtual ~FRHI();

  FRHI(FRHI &) = delete;
  FRHI &operator=(FRHI &) = delete;

public:
  inline static FRHI &Get() { return *sInstance; }

  static void Init();
  static void Shutdown();

  virtual FRHISamplerStateRef CreateSamplerState(const FRHISamplerStateInitializer &Initializer) = 0;
  virtual FRHIRasterizerStateRef CreateRasterizerState(const FRHIRasterizerStateInitializer &Initializer) = 0;
  virtual FRHIDepthStencilStateRef CreateDepthStencilState(const FRHIDepthStencilStateInitializer &Initializer) = 0;
  virtual FRHIBlendStateRef CreateBlendState(const FRHIBlendStateInitializer &Initializer) = 0;
  virtual FRHIVertexDeclarationRef CreateVertexDeclaration(const FRHIVertexDeclarationElementList &Elements) = 0;

  virtual FRHIPixelShaderRef CreatePixelShader() = 0;
  virtual FRHIVertexShaderRef CreateVertexShader() = 0;
  virtual FRHIGeometryShaderRef CreateGeometryShader() = 0;

  virtual FRHIBufferRef CreateBuffer(const FRHIBufferDescription &Description, ERHIAccess Access) = 0;
  virtual FRHIUniformBufferRef CreateUniformBuffer(const void *InitialContents, const FRHIUniformBufferLayout &Layout, ERHIUniformBufferUsage Usage) = 0;
  virtual FRHIStagingBufferRef CreateStagingBuffer() = 0;

  virtual FRHITextureRef CreateTexture(const FRHITextureDescription &Description) = 0;

  virtual FRHIGraphicsPipelineStateRef CreateGraphicsPipelineState(const FRHIGraphicsPipelineStateInitializer &Initializer) = 0;

  virtual FRHIShaderResourceViewRef CreateShaderResourceView(FRHIViewableResourceRef Resource, const FRHIViewDescription &Description) = 0;
  virtual FRHIUnorderedAccessViewRef CreateUnorderedAccessView(FRHIViewableResourceRef Resource, const FRHIViewDescription &Description) = 0;

  virtual void Submit(FRHICommandList &CommandList) final;

  virtual void InitImGui() = 0;
  virtual void ShutdownImGui() = 0;
  virtual void ImGuiNewFrame() = 0;
  virtual void SubmitImGuiDrawData(ImDrawData *DrawData) = 0;

  virtual void BeginDrawing() = 0;
  virtual void EndDrawing() = 0;
  virtual void FramebufferResized() = 0;

  virtual void WaitForGPUIdle() = 0;

  virtual IRHICommandContext *GetCommandContext() = 0;

private:
  inline static FRHI *sInstance;
};