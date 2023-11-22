#include "Renderer.h"

#include "Log.h"

#ifdef VULKAN
#include "VulkanRenderer.h"
#endif

Unique<Renderer> MakeRenderer()
{
    #ifdef VULKAN
    return MakeUnique<VulkanRenderer>();
    #endif

    SK_CORE_CRITICAL("Invalid Renderer API");
    return nullptr;
}
