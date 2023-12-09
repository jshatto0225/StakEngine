#include "Renderer.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

struct QueueFamilyIndices final
{
    std::optional<u32> graphicsFamily;

    inline bool IsComplete()
    {
        return graphicsFamily.has_value();
    }
};

class VulkanRenderer final : public Renderer
{
public:
    VulkanRenderer();

    void Init() override;
    void Shutdown() override;

    void Draw() override;
    void Clear() override;
    void SetClearColor() override;
    ~VulkanRenderer() override;

private:
    std::vector<const char*> GetRequiredExtensions();
    void CreateInstance();
    void SetupDebugMessenger();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo);
    VkResult CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pDebugCreateInfo,
                                          const VkAllocationCallbacks* pAllocator);
    void DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator);
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData);
    static bool IsDeviceSuitable(VkPhysicalDevice vkPhysicalDevice);
    static i32 RateDeviceSuitability(VkPhysicalDevice vkPhysicalDevice);
    static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice vkPhysicalDevice);

private:
    VkInstance mVkInstance;
    VkDebugUtilsMessengerEXT mVkDebugMessenger;
    VkPhysicalDevice mVkPhysicalDevice;
    VkDevice mVkDevice;
    VkPhysicalDeviceFeatures mVkDeviceFeatures;
    VkQueue mVkGraphicsQueue;
};
