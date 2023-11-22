#include <Vulkan/vulkan.h>
#include <vector>

const std::vector<const char*> kValidationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef SK_DEBUG
const bool kEnableValidationLayers = true;
#else
const bool kEnableValidationLaters = false;
#endif

bool CheckValidationLayerSupport();
