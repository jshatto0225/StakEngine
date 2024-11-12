#pragma once

#include "Types.h"
#include "Window.h"

#include <imgui.h>

namespace Stak {

typedef u64 HRHIResource;

enum class ERHIDataFormat {
  R8_SINT,
  R8_UINT,
  R8G8_SINT,
  R8G8_UINT,
  R8G8B8_SINT,
  R8G8B8_UINT,
  R8G8B8A8_SINT,
  R8G8B8A8_UINT,
  
  R16_SINT,
  R16_UINT,
  R16G16_SINT,
  R16G16_UINT,
  R16G16B16_SINT,
  R16G16B16_UINT,
  R16G16B16A16_SINT,
  R16G16B16A16_UINT,
  
  R32_SINT,
  R32_UINT,
  R32_FLOAT,
  R32G32_SINT,
  R32G32_UINT,
  R32G32_FLOAT,
  R32G32B32_SINT,
  R32G32B32_UINT,
  R32G32B32_FLOAT,
  R32G32B32A32_SINT,
  R32G32B32A32_UINT,
  R32G32B32A32_FLOAT,
  
  R64_SINT,
  R64_UINT,
  R64_FLOAT,
  R64G64_SINT,
  R64G64_FLOAT,
  R64G64_UINT,
  R64G64B64_SINT,
  R64G64B64_UINT,
  R64G64B64_FLOAT,
  R64G64B64A64_SINT,
  R64G64B64A64_UINT,
  R64G64B64A64_FLOAT,
};

enum class ERHIReciptType {
  RENDER,
  UPLOAD,
  COMPUTE,
};

struct IRHIRecipt {
  ERHIReciptType type;
};

class IRHIDevice;
class IRHIGraphicsContext;

class IRHI {
public:
  virtual ~IRHI() = default;

  static Scope<IRHI> create();

  virtual Ref<IRHIDevice> createDevice(Ref<Window> window) = 0;
  virtual void initImGui(Ref<IRHIDevice> device) = 0;
  virtual void imGuiNewFrame() = 0;
  virtual void shutdownImGui() = 0;
  virtual void renderImGuiDrawData(ImDrawData *data, Ref<IRHIGraphicsContext> context) = 0;
};

} // namespace Stak