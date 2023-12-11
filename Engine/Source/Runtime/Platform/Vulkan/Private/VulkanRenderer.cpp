#include "VulkanRenderer.h"

#include <stdexcept>
#include <vector>
#include <array>
#include <map>
#include <optional>
#include <algorithm>
#include <set>

#include "Types.h"
#include "Log.h"
#include "ValidationLayers.h"

VulkanRenderer::VulkanRenderer(void* window)
{
    Init(window);
}

void VulkanRenderer::Init(void* window)
{
    SK_CORE_INFO("Vulkan Initialization:\n");
    CreateInstance();
    if (kEnableValidationLayers) 
    {
        SetupDebugMessenger();
    }
    AddWindow(window);
    PickPhysicalDevice();
    CreateLogicalDevice();
    mRunning = true;
}

void VulkanRenderer::Shutdown()
{
    if(mRunning)
    {
        SK_CORE_WARN("Renderer Shutdown");
        mRunning = false;
        vkDestroyDevice(mVkDevice, nullptr);
        if (kEnableValidationLayers)
        {
            DestroyDebugUtilsMessengerEXT(nullptr);
        }
        for(VkSurfaceKHR surface : mVkSurfaces)
        {
            vkDestroySurfaceKHR(mVkInstance, surface, nullptr);
        }
        mVkSurfaces.clear();
        mWindowSurfaceMap.clear();
        vkDestroyInstance(mVkInstance, nullptr);
    }
}

void VulkanRenderer::CreateInstance()
{
    if(kEnableValidationLayers && !CheckValidationLayerSupport())
    {
        throw std::runtime_error("ValidationLayers requested but not found");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Application"; // TODO: pass app name to CreateInstance
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "StakEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    
    if(kEnableValidationLayers)
    {
        createInfo.enabledExtensionCount = (u32)kValidationLayers.size();
        createInfo.ppEnabledLayerNames = kValidationLayers.data();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledExtensionCount = 0;
        createInfo.pNext = nullptr;
    }

    std::vector<const char*> extensions = GetRequiredExtensions();
    createInfo.enabledExtensionCount = (u32)extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    for(const char* extension : extensions)
    {
        SK_CORE_INFO("Extension Name: %s", extension);
    }

    if (vkCreateInstance(&createInfo, nullptr, &mVkInstance))
    {
        throw std::runtime_error("Failed to create vukan instance");
    }
}

void VulkanRenderer::SetupDebugMessenger()
{
    if (!kEnableValidationLayers)
    {
        return;
    }
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    PopulateDebugMessengerCreateInfo(debugCreateInfo);

    if (CreateDebugUtilsMessengerEXT(&debugCreateInfo, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("ERROR: Failed to setup debug messenger");
    }
}

VulkanRenderer::~VulkanRenderer()
{
    if(mRunning)
    {
        Shutdown();
    }
}

void VulkanRenderer::Draw()
{

}

void VulkanRenderer::Clear()
{

}

void VulkanRenderer::SetClearColor()
{

}

std::vector<const char*>  VulkanRenderer::GetRequiredExtensions()
{
    std::vector<const char*> extensions = {};
    if (kEnableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    #ifdef WIN32
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    #endif
    return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                void* pUserData)
{
    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    {
        SK_CORE_TRACE("Validation Layer: %s", pCallbackData->pMessage);
    } break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    {
        SK_CORE_WARN("Validation Layer: %s", pCallbackData->pMessage);
    } break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    {
        SK_CORE_ERROR("Validation Layer: %s", pCallbackData->pMessage);
    } break;
    }
    return VK_FALSE;
}

VkResult VulkanRenderer::CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pDebugCreateInfo,
                                                      const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mVkInstance, "vkCreateDebugUtilsMessengerEXT");
    if (func)
    {
        return func(mVkInstance, pDebugCreateInfo, pAllocator, &mVkDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VulkanRenderer::DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mVkInstance, "vkDestroyDebugUtilsMessengerEXT");
    if (func)
    {
        func(mVkInstance, mVkDebugMessenger, pAllocator);
    }
}

void VulkanRenderer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo)
{
    debugCreateInfo = {};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 
                                    | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT 
                                    | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT 
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT 
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = DebugCallback;
}

void VulkanRenderer::PickPhysicalDevice()
{
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(mVkInstance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("ERROR: Failed to find gpus with vulkan support");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(mVkInstance, &deviceCount, devices.data());

    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto& device : devices)
    {
        int score = RateDeviceSuitability(device);
        candidates.insert(std::make_pair(score, device));
    }

    if (candidates.rbegin()->first > 0)
    {
        mVkPhysicalDevice = candidates.rbegin()->second;
    }
    else
    {
        throw std::runtime_error("ERROR: Failed to find suitable GPU");
    }
}

bool VulkanRenderer::IsDeviceSuitable(VkPhysicalDevice vkPhysicalDevice)
{
    QueueFamilyIndices indices = FindQueueFamilies(vkPhysicalDevice);
    VkBool32 presentSupport = false;
    return indices.IsComplete();
}

i32 VulkanRenderer::RateDeviceSuitability(VkPhysicalDevice vkPhysicalDevice)
{
    VkPhysicalDeviceProperties deviceProperties = {};
    vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures = {};
    vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &deviceFeatures);

    int score = 0;
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }

    score += deviceProperties.limits.maxImageDimension2D;
    if (!deviceFeatures.geometryShader)
    {
        return 0;
    }

    SK_CORE_TRACE("Found Device: %s, With Score: %d", deviceProperties.deviceName, score);

    return score;
}

QueueFamilyIndices VulkanRenderer::FindQueueFamilies(VkPhysicalDevice vkPhysicalDevice)
{
    QueueFamilyIndices indices = {};

    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

    for (i32 i = 0; i < queueFamilies.size(); i++)
    {
        if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        // TODO: Handle all windows
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, mVkSurfaces[0], &presentSupport);
        if(presentSupport)
        {
            indices.presentFamily = i;
        }

        if (indices.IsComplete())
        {
            break;
        }
    }

    return indices;
}

void VulkanRenderer::CreateLogicalDevice()
{
    QueueFamilyIndices indices = FindQueueFamilies(mVkPhysicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<u32> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    f32 queuePriority = 1.0f;
    for(u32 queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
    
    deviceCreateInfo.enabledExtensionCount = 0;

    if(kEnableValidationLayers)
    {
        deviceCreateInfo.enabledLayerCount = kValidationLayers.size();
        deviceCreateInfo.ppEnabledExtensionNames = kValidationLayers.data();
    }
    else
    {
        deviceCreateInfo.enabledExtensionCount = 0;
    }

    if(vkCreateDevice(mVkPhysicalDevice, &deviceCreateInfo, nullptr, &mVkDevice))
    {
        throw std::runtime_error("ERROR: Failed to create logical device");
    }

    vkGetDeviceQueue(mVkDevice, indices.graphicsFamily.value(), 0, &mVkGraphicsQueue);
    vkGetDeviceQueue(mVkDevice, indices.presentFamily.value(), 0, &mVkPresentQueue);
}

void VulkanRenderer::AddWindow(void* window)
{
    // TODO: I got platform code in my platform code
    #ifdef WIN32
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hwnd = (HWND)window;
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
    VkSurfaceKHR newSurface;
    if(vkCreateWin32SurfaceKHR(mVkInstance, &surfaceCreateInfo, nullptr, &newSurface) != VK_SUCCESS)
    {
        throw std::runtime_error("ERROR: Failed to create window surface");
    }
    mVkSurfaces.push_back(newSurface);
    mWindowSurfaceMap[window] = newSurface;
    #endif
}

void VulkanRenderer::RemoveWindow(void* window)
{
    VkSurfaceKHR surface = mWindowSurfaceMap[window];
    vkDestroySurfaceKHR(mVkInstance, surface, nullptr);
    mWindowSurfaceMap.erase(window);
    std::vector<VkSurfaceKHR>::iterator it = std::find(mVkSurfaces.begin(), mVkSurfaces.end(), surface);
    mVkSurfaces.erase(it);
}
