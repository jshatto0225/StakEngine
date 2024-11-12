#pragma once

#include "IRHIBuffer.h"

namespace Stak {

class VulkanRHIBuffer : public IRHIBuffer {
public:
  VulkanRHIBuffer(VkDevice, BufferDescription &desc);
  ~VulkanRHIBuffer();

  u32 getStride();
  u32 getSize();
  u32 getNumElements();

  void setApiResource(HRHIResource resource);
  HRHIResource getApiResource();
};

} // namespace Stak