#pragma once

#include "Types.h"
#include "Window.h"

namespace Stak {

typedef u64 HRHIResource;

enum class ERHIDataType {
  FLOAT,
  FLOAT2,
  FLOAT3,
  FLOAT4,
  MAT2,
  MAT3,
  MAT4,
  INT,
  INT2,
  INT3,
  INT4,
  BOOl
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

  virtual void init() = 0;
  virtual void shutdown() = 0;
  virtual Ref<IRHIDevice> createDevice(Ref<Window> window) = 0;
  virtual void initImGui(Ref<IRHIDevice> device, Ref<IRHIGraphicsContext> context) = 0;
  virtual void imGuiNewFrame() = 0;
};

} // namespace Stak