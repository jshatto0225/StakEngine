#include "RHI.h"

#ifdef SK_VULKAN
#include "VulkanRHI.h"
#endif

RHI rhi;

bool rhi_init() {
    return vulkan_init(&rhi);
}

void rhi_shutdown() {
    vulkan_shutdown();
}