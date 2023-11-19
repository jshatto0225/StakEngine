#include "Renderer.h"

#include <vulkan/vulkan.h>

class VulkanRenderer : public Renderer
{
public:
    void Init() override;
    void Draw() override;
    void Clear() override;
    void SetClearColor() override;

private:
    VkInstance instance;
};