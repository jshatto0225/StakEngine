#include "VulkanRenderer.h"

#include <stdexcept>
#include <vector>
#include <array>
#include <map>
#include <optional>

#include "Types.h"
#include "Log.h"
#include "ValidationLayers.h"

VulkanRenderer::VulkanRenderer()
{
    Init();
}

void VulkanRenderer::Init()
{
    SK_CORE_INFO("Vulkan Initialization:\n");
    CreateInstance();
    if (kEnableValidationLayers) 
    {
        SetupDebugMessenger();
    }
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

    auto extensions = GetRequiredExtensions();
    createInfo.enabledExtensionCount = (u32)extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

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

    mVkDeviceFeatures = {}; // TODO:
    
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    f32 queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pEnabledFeatures = &mVkDeviceFeatures;
    deviceCreateInfo.enabledExtensionCount = 0;

    if (kEnableValidationLayers)
    {
        deviceCreateInfo.enabledLayerCount = (u32)kValidationLayers.size();
        deviceCreateInfo.ppEnabledLayerNames = kValidationLayers.data();
    }

    if (vkCreateDevice(mVkPhysicalDevice, &deviceCreateInfo, nullptr, &mVkDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("ERORR: Failed to create logical device");
    }

    vkGetDeviceQueue(mVkDevice, indices.graphicsFamily.value(), 0, &mVkGraphicsQueue);
}
