#include "VulkanRHIDevice.h"

#ifdef SK_GLFW
#include <GLFW/glfw3.h>
#endif

#include "Log.h"
#include "VulkanRHI.h"
#include "VulkanbRHIBuffer.h"
#include "VulkanRHITexture.h"
#include "VulkanRHIShader.h"
#include "VulkanRHIPipeline.h"
#include "VulkanRHIContext.h"

namespace Stak {

VulkanRHIDevice::VulkanRHIDevice(VkInstance instance, Ref<Window> window) : mInstance(instance) {
  // SURFACE
#ifdef SK_GLFW
  VkResult r = glfwCreateWindowSurface(
    mInstance,
    reinterpret_cast<GLFWwindow *>(window->getNativeHandle()),
    NULL,
    &mSurface
  );
  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("Falied to create vulkan surface");
  }
#endif
  
  // DEVICE
  u32 deviceCount = 0;
  vkEnumeratePhysicalDevices(mInstance, &deviceCount, NULL);
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

  bool deviceFound = false;
  for (u32 i = 0; i < deviceCount; i++) {
    bool isDeviceSuitable = false;
    VulkanQueueFamilies indices = findQueueFamilies(devices[i]);

    u32 deviceExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties(
      devices[i],
      NULL,
      &deviceExtensionCount,
      NULL
    );
    std::vector<VkExtensionProperties> availableExtensions(
      deviceExtensionCount
    );

    vkEnumerateDeviceExtensionProperties(
      devices[i],
      NULL,
      &deviceExtensionCount,
      availableExtensions.data()
    );

    bool extensionsSupported = false;

    for (u32 k = 0; k < deviceExtensionCount; k++) {
      if (
        std::strcmp(
          availableExtensions[k].extensionName,
          VK_KHR_SWAPCHAIN_EXTENSION_NAME
        )
        ) {
        extensionsSupported = true;
        break;
      }
    }

    bool swapchainAdequate = false;
    if (extensionsSupported) {
      VulkanSwapchainSupport support = getSwapchainSupport(devices[i]);
      swapchainAdequate =
        support.presentModes.size() > 0 &&
        support.formats.size() > 0;
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(devices[i], &supportedFeatures);

    isDeviceSuitable =
      extensionsSupported &&
      swapchainAdequate &&
      indices.graphicsFamily.has_value() &&
      indices.presentFamily.has_value() &&
      supportedFeatures.samplerAnisotropy;

    if (isDeviceSuitable) {
      mPhysicalDevice = devices[i];
      deviceFound = true;
      break;
    }
  }

  if (!deviceFound) {
    SK_LOG_ERROR("Failed to find suitable physical device");
  }

  VulkanQueueFamilies indices = findQueueFamilies(mPhysicalDevice);

  std::vector<VkDeviceQueueCreateInfo> queueInfos;
  std::set<u32> uniqueFamilies = {
    indices.graphicsFamily.value(),
    indices.presentFamily.value()
  };
  f32 queuePriority = 1.0f;
  for (u32 queueFamily : uniqueFamilies) {
    VkDeviceQueueCreateInfo queueInfo = {};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = queueFamily;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePriority;
    queueInfos.push_back(queueInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures = {};
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo deviceInfo = {};
  deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  deviceInfo.queueCreateInfoCount = queueInfos.size();
  deviceInfo.pQueueCreateInfos = queueInfos.data();
  deviceInfo.pEnabledFeatures = &deviceFeatures;
  deviceInfo.enabledExtensionCount = DEVICE_EXTENSIONS.size();
  deviceInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

  if (ENABLE_VALIDATION_LAYERS) {
    deviceInfo.enabledLayerCount = VALIDATION_LAYERS.size();
    deviceInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
  }

  if (
    vkCreateDevice(mPhysicalDevice, &deviceInfo, NULL, &mDevice) !=
    VK_SUCCESS
  ) {
    SK_LOG_ERROR("Failed to create vulkan device");
  }

  vkGetDeviceQueue(
    mDevice,
    indices.graphicsFamily.value(),
    0,
    &mGraphicsQueue
  );
  vkGetDeviceQueue(
    mDevice,
    indices.presentFamily.value(),
    0,
    &mPresentQueue
  );

  mGraphicsQueueFamily = indices.graphicsFamily.value();

  createSwapchain(window);
}

VulkanRHIDevice::~VulkanRHIDevice() {
  vkDestroySurfaceKHR(mInstance, mSurface, NULL);
}

void VulkanRHIDevice::createSwapchain(Ref<Window> window) {
  VulkanSwapchainSupport support = getSwapchainSupport(
    mPhysicalDevice
  );

  VkSurfaceFormatKHR format = {};
  for (u32 i = 0; i < support.formats.size(); i++) {
    if (
      support.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
      support.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
    ) {
      format = support.formats[i];
      break;
    }
  }

  VkPresentModeKHR presentMode = {};
  for (u32 i = 0; i < support.presentModes.size(); i++) {
    if (support.presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      presentMode = support.presentModes[i];
      break;
    }
  }

  if (support.capabilities.currentExtent.width != UINT32_MAX) {
    mExtent = support.capabilities.currentExtent;
  }
  else {
    auto [width, height] = window->getSize();

    VkExtent2D extent = { width, height };

    extent.width = std::clamp(
      extent.width,
      support.capabilities.minImageExtent.width,
      support.capabilities.maxImageExtent.width
    );
    extent.height = std::clamp(
      extent.height,
      support.capabilities.minImageExtent.height,
      support.capabilities.maxImageExtent.height
    );

    mExtent = extent;
  }

  u32 imageCount = support.capabilities.maxImageCount + 1;

  if (
    support.capabilities.maxImageCount > 0 &&
    imageCount > support.capabilities.maxImageCount
  ) {
    imageCount = support.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR swapchainInfo = {};
  swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainInfo.surface = mSurface;
  swapchainInfo.minImageCount = imageCount;
  swapchainInfo.imageFormat = format.format;
  swapchainInfo.imageColorSpace = format.colorSpace;
  swapchainInfo.imageExtent = mExtent;
  swapchainInfo.imageArrayLayers = 1;
  swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  mSwapchainImageFormat = format;

  VulkanQueueFamilies indices = findQueueFamilies(mPhysicalDevice);

  u32 queueFamilyIndices[] = {
      indices.graphicsFamily.value(),
      indices.presentFamily.value()
  };

  if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchainInfo.queueFamilyIndexCount = 2;
    swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
  }
  else {
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  swapchainInfo.preTransform = support.capabilities.currentTransform;
  swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainInfo.presentMode = presentMode;
  swapchainInfo.clipped = VK_TRUE;
  swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
  VkResult r = vkCreateSwapchainKHR(
    mDevice,
    &swapchainInfo,
    NULL,
    &mSwapchain
  );

  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create swapchain\n");
    return;
  }

  mSwapchainImages.resize(imageCount);

  vkGetSwapchainImagesKHR(
    mDevice,
    mSwapchain,
    &imageCount,
    mSwapchainImages.data()
  );
}

VkImageView VulkanRHIDevice::createImageView(VkImage image, VkFormat format) {
  VkImageViewCreateInfo viewInfo = { };
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = format;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  VkImageView imageView;
  VkResult r = vkCreateImageView(
    mDevice,
    &viewInfo,
    NULL,
    &imageView
  );
  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("failed to create image view!\n");
  }

  return imageView;
}

void VulkanRHIDevice::createImageViews() {
  mSwapchainImageViews.resize(mSwapchainImages.size());

  for (u32 i = 0; i < mSwapchainImages.size(); i++) {
    mSwapchainImageViews[i] =
      createImageView(
        mSwapchainImages[i],
        mSwapchainImageFormat.format
      );
  }
}

VulkanRHIDevice::VulkanQueueFamilies VulkanRHIDevice::findQueueFamilies(
  VkPhysicalDevice device
) {
  VulkanQueueFamilies indices;

  u32 queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(
    device,
    &queueFamilyCount,
    queueFamilies.data()
  );

  for (u32 i = 0; i < queueFamilyCount; i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    VkBool32 presentSupport = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(
      device,
      i,
      mSurface,
      &presentSupport
    );
    if (presentSupport) {
      indices.presentFamily = i;
    }
    if (
      indices.presentFamily.has_value() &&
      indices.graphicsFamily.has_value()
    ) {
      break;
    }
  }

  return indices;
}

VulkanRHIDevice::VulkanSwapchainSupport VulkanRHIDevice::getSwapchainSupport(VkPhysicalDevice device) {
  VulkanSwapchainSupport support = {};

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &support.capabilities);

  u32 formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, NULL);
  support.formats.resize(formatCount);

  vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, support.formats.data());

  u32 presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, NULL);
  support.presentModes.resize(presentModeCount);

  vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, support.presentModes.data());

  return support;
}

void VulkanRHIDevice::recreateSwapchain(Ref<Window> window) {
  auto [width, height] = window->getSize();

  while (width == 0 || height == 0) {
    WindowSizeData data = window->getSize();
    width = data.width;
    height = data.height;
  }

  vkDeviceWaitIdle(mDevice);

  for (u32 i = 0; i < mSwapchainImages.size(); i++) {
    vkDestroyImageView(
      mDevice,
      mSwapchainImageViews[i],
      NULL
    );
  }

  vkDestroySwapchainKHR(
    mDevice,
    mSwapchain,
    NULL
  );

  createSwapchain(window);
  createImageViews();
}

void VulkanRHIDevice::processWindowChanges(Ref<Window> window) {
  recreateSwapchain(window);
  // TODO: Notify framebuffers that recreation is required
}

Ref<IRHIBuffer> VulkanRHIDevice::createBuffer(RHIBufferDescription &bufferDesc) {
  return createRef<VulkanRHIBuffer>(mDevice, bufferDesc);
}

Ref<IRHITexture> VulkanRHIDevice::createTexture(RHITextureDescription &textureDesc) {
  return createRef<VulkanRHITexture>(mDevice, textureDesc);
}

Ref<IRHIShader> VulkanRHIDevice::createShader(RHIShaderDescription &shaderDesc) {
  return createRef<VulkanRHIShader>(mDevice, shaderDesc);
}

Ref<IRHIPipeline> VulkanRHIDevice::createPipeline(RHIPipelineDescription &pipelineDesc) {
  return createRef<VulkanRHIPipeline>(mDevice, pipelineDesc);
}

Ref<IRHIGraphicsContext> VulkanRHIDevice::createGraphicsContext() {
  return createRef<VulkanRHIGraphicsContext>(mDevice, mGraphicsQueueFamily);
}

Scope<IRHIRecipt> VulkanRHIDevice::submitWork(Ref<IRHIContext> context) {
  Scope<VulkanRHIRecipt> recipt = createScope<VulkanRHIRecipt>();

  switch (context->getType()) {
  case ERHIContextType::GRAPHICS: {
    Ref<VulkanRHIGraphicsContext> vulkanContext = std::static_pointer_cast<VulkanRHIGraphicsContext>(context);
    recipt->fences = vulkanContext->getFences();
    break;
  }
  case ERHIContextType::COMPUTE: {
    Ref<VulkanRHIComputeContext> vulkanContext = std::static_pointer_cast<VulkanRHIComputeContext>(context);
    recipt->fences = vulkanContext->getFences();
    break;
  }
  case ERHIContextType::UPLOAD: {
    Ref<VulkanRHIComputeContext> vulkanContext = std::static_pointer_cast<VulkanRHIComputeContext>(context);
    recipt->fences = vulkanContext->getFences();
    break;
  }
  }

  return recipt;
}

void VulkanRHIDevice::waitOnWork(Scope<IRHIRecipt> recipt) {
  Scope<VulkanRHIRecipt> vulkanRecipt(static_cast<VulkanRHIRecipt *>(recipt.release()));
  vkWaitForFences(mDevice, vulkanRecipt->fences.size(), vulkanRecipt->fences.data(), VK_TRUE, UINT64_MAX);
}

void VulkanRHIDevice::present() {
  
}

} // namespace Stak