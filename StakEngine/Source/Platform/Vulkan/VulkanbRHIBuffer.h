#pragma once

#include "IRHIBuffer.h"

namespace Stak {

class VulkanRHIBuffer : public IRHIBuffer {
public:
  VulkanRHIBuffer(VkDevice, RHIBufferDescription &desc);
  ~VulkanRHIBuffer();

  u32 getStride();
  u32 getSize();
  u32 getNumElements();
};

} // namespace Stak