#include "Renderer.h"

#ifdef WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#endif
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include <unordered_map>

struct QueueFamilyIndices final
{
    std::optional<u32> graphicsFamily;
    std::optional<u32> presentFamily;

    inline bool IsComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class VulkanRenderer final : public Renderer
{
public:
    VulkanRenderer(void* window);
    ~VulkanRenderer() override;

    void Init(void* window) override;
    void Shutdown() override;

    void Draw() override;
    void Clear() override;
    void SetClearColor() override;
    void AddWindow(void* window) override;
    void RemoveWindow(void* window) override;

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
    bool IsDeviceSuitable(VkPhysicalDevice vkPhysicalDevice);
    i32 RateDeviceSuitability(VkPhysicalDevice vkPhysicalDevice);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice vkPhysicalDevice);

private:
    VkInstance mVkInstance;
    VkDebugUtilsMessengerEXT mVkDebugMessenger;
    VkPhysicalDevice mVkPhysicalDevice;
    VkDevice mVkDevice;
    VkPhysicalDeviceFeatures mVkDeviceFeatures;
    VkQueue mVkGraphicsQueue;
    VkQueue mVkPresentQueue;
    std::vector<VkSurfaceKHR> mVkSurfaces;
    std::unordered_map<void*, VkSurfaceKHR> mWindowSurfaceMap;
};
