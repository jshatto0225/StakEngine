#include "VulkanPlatform.h"

#include <glfw/glfw3.h>

std::vector<const char *> platform_get_required_extensions() {
    u32 count = 0;
    const char **extenions = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char *> ext(extensions, count);
    return ext;
}