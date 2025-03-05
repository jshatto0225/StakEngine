#include "VulkanDevice.h"

#include "VulkanRHI.h"
#include "VulkanCommandContext.h"
#include "VulkanBuffer.h"
#include "VulkanShader.h"
#include "Platform.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptorSetLayout.h"

#include <backends/imgui_impl_vulkan.h>

#ifdef SK_GLFW
#include "GLFWWindow.h"
#endif

#include <cmath>
#include <set>

FVulkanDevice::FVulkanDevice(TRef<IWindow> Window, VkInstance Instance) : Window(Window), Instance(Instance) {
#ifdef SK_GLFW
    TRef<FGLFWWindow> glfwWindow = std::static_pointer_cast<FGLFWWindow>(Window);
    glfwCreateWindowSurface(Instance, glfwWindow->GetGLFWWindow(), nullptr, &Surface);
#endif

    FUInt32 DeviceCount = 0;
    vkEnumeratePhysicalDevices(Instance, &DeviceCount, nullptr);
    std::vector<VkPhysicalDevice> Devices(DeviceCount);
    vkEnumeratePhysicalDevices(Instance, &DeviceCount, Devices.data());

    FBool DeviceFound = false;
    for (FUInt32 DeviceIndex = 0; DeviceIndex < DeviceCount; DeviceIndex++) {
        FBool IsDeviceSuitable = false;
        VulkanQueueFamilies Indices = FindQueueFamilies(Devices[DeviceIndex], Surface);

        FUInt32 DeviceExtensionCount = 0;
        vkEnumerateDeviceExtensionProperties(Devices[DeviceIndex], nullptr, &DeviceExtensionCount, nullptr);
        std::vector<VkExtensionProperties> AvailableExtensions(DeviceExtensionCount);

        vkEnumerateDeviceExtensionProperties(Devices[DeviceIndex], nullptr, &DeviceExtensionCount, AvailableExtensions.data());

        FBool ExtensionsSupported = false;

        for (FUInt32 DeviceExtensionIndex = 0; DeviceExtensionIndex < DeviceExtensionCount; DeviceExtensionIndex++) {
            if (std::strcmp(AvailableExtensions[DeviceExtensionIndex].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) != 0) {
                ExtensionsSupported = true;
                break;
            }
        }

        FBool SwapchainAdequate = false;
        if (ExtensionsSupported) {
            VulkanSwapchainSupport Support = GetSwapchainSupport(Devices[DeviceIndex], Surface);
            SwapchainAdequate = Support.PresentModes.size() > 0 && Support.Formats.size() > 0;
        }

        VkPhysicalDeviceFeatures SupportedFeatures;
        vkGetPhysicalDeviceFeatures(Devices[DeviceIndex], &SupportedFeatures);

        IsDeviceSuitable = ExtensionsSupported && SwapchainAdequate && Indices.GraphicsFamily.has_value() && Indices.PresentFamily.has_value() && SupportedFeatures.samplerAnisotropy;

        if (IsDeviceSuitable) {
            GPU = Devices[DeviceIndex];
            DeviceFound = true;
            break;
        }
    }

    if (!DeviceFound) {
        SK_LOG_ERROR("Failed to find suitable physical device");
    }

    VulkanQueueFamilies Indices = FindQueueFamilies(GPU, Surface);
    GraphicsQueueIndex = Indices.GraphicsFamily.value();
    PresentQueueIndex = Indices.PresentFamily.value();

    std::set<FUInt32> UniqueFamilies = {
        Indices.GraphicsFamily.value(),
        Indices.PresentFamily.value()
    };

    std::vector<VkDeviceQueueCreateInfo> QueueInfos;
    QueueInfos.reserve(UniqueFamilies.size());

    FFloat QueuePriority = 1.0f;
    for (FUInt32 QueueFamily : UniqueFamilies) {
        VkDeviceQueueCreateInfo queueInfo = {};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = QueueFamily;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &QueuePriority;
        QueueInfos.push_back(queueInfo);
    }

    VkPhysicalDeviceFeatures DeviceFeatures = {};
    DeviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo DeviceInfo = {};
    DeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    VkPhysicalDeviceDynamicRenderingFeatures DynamicRenderingFeatures = {};
    DynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    DynamicRenderingFeatures.dynamicRendering = VK_TRUE;
    DeviceInfo.pNext = &DynamicRenderingFeatures;

    VkPhysicalDeviceMultiviewFeatures MultiviewFeatures = {};
    MultiviewFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES;
    MultiviewFeatures.multiview = VK_TRUE;

    DynamicRenderingFeatures.pNext = &MultiviewFeatures;

    VkPhysicalDeviceSynchronization2Features Sync2Features = {};
    Sync2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    Sync2Features.synchronization2 = VK_TRUE;

    MultiviewFeatures.pNext = &Sync2Features;

    DeviceInfo.queueCreateInfoCount = static_cast<FUInt32>(QueueInfos.size());
    DeviceInfo.pQueueCreateInfos = QueueInfos.data();
    DeviceInfo.pEnabledFeatures = &DeviceFeatures;
    DeviceInfo.enabledExtensionCount = static_cast<FUInt32>(DEVICE_EXTENSIONS.size());
    DeviceInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

    if (ENABLE_VALIDATION_LAYERS) {
        DeviceInfo.enabledLayerCount = static_cast<FUInt32>(VALIDATION_LAYERS.size());
        DeviceInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    }

    CHECK_VK_ERR(vkCreateDevice(GPU, &DeviceInfo, nullptr, &Device), "Failed to create vulkan device");

    vkGetDeviceQueue(Device, Indices.PresentFamily.value(), 0, &PresentQueue);
    vkGetDeviceQueue(Device, Indices.PresentFamily.value(), 0, &GraphicsQueue);

    CreateSwapchain();

    VkSemaphoreCreateInfo SemaphoreInfo = {};
    SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo FenceInfo = {};
    FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (FUInt32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        CHECK_VK_ERR(vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &ImageAvailableSemaphores[i]), "Failed to create vulkan semaphore");

        CHECK_VK_ERR(vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &RenderFinishedSemaphores[i]), "Failed to create vulkan semaphore");

        CHECK_VK_ERR(vkCreateFence(Device, &FenceInfo, nullptr, &InFlightFences[i]), "Failed to create vulkan fence");
    }
}

void FVulkanDevice::RecreateSwapchain() {
    // TODO: Not sure what to do here, maybe try recreation every app loop, 
    // but then every rendering object created by this device would have to know to
    // wait for the swapchain to be successfully recreated
    auto [Width, Height] = Window->GetFramebufferSize();

    while (Width == 0 || Height == 0) {
        PlatformPollEvents();
        FWindowSizeData data = Window->GetFramebufferSize();
        Width = data.Width;
        Height = data.Height;
    }

    WaitForGPUIdle();

    Backbuffer->Shutdown();

    vkDestroySwapchainKHR(Device, Swapchain, nullptr);

    CreateSwapchain();
}

void FVulkanDevice::CreateSwapchain() {
    VulkanSwapchainSupport Support = GetSwapchainSupport(GPU, Surface);

    VkSurfaceFormatKHR Format = {};
    for (FUInt32 FormatIndex = 0; FormatIndex < Support.Formats.size(); FormatIndex++) {
        if (Support.Formats[FormatIndex].format == VK_FORMAT_B8G8R8A8_SRGB && Support.Formats[FormatIndex].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            Format = Support.Formats[FormatIndex];
            break;
        }
    }

    VkPresentModeKHR PresentMode = {};
    for (FUInt32 PresentModeIndex = 0; PresentModeIndex < Support.PresentModes.size(); PresentModeIndex++) {
        if (Support.PresentModes[PresentModeIndex] == VK_PRESENT_MODE_MAILBOX_KHR) {
            PresentMode = Support.PresentModes[PresentModeIndex];
            break;
        }
    }

    VkExtent2D SwapchainExtent = {};

    if (Support.Capabilities.currentExtent.width != UINT32_MAX) {
        SwapchainExtent = Support.Capabilities.currentExtent;
    }
    else {
        auto [Width, Height] = Window->GetFramebufferSize();

        VkExtent2D Extent = {
            static_cast<FUInt32>(Width),
            static_cast<FUInt32>(Height)
        };

        Extent.width = std::clamp(Extent.width, Support.Capabilities.minImageExtent.width, Support.Capabilities.maxImageExtent.width);
        Extent.height = std::clamp(Extent.height, Support.Capabilities.minImageExtent.height, Support.Capabilities.maxImageExtent.height);

        SwapchainExtent = Extent;
    }

    FUInt32 ImageCount = Support.Capabilities.maxImageCount + 1;

    if (Support.Capabilities.maxImageCount > 0 && ImageCount > Support.Capabilities.maxImageCount) {
        ImageCount = Support.Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR SwapchainInfo = {};
    SwapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapchainInfo.surface = Surface;
    SwapchainInfo.minImageCount = ImageCount;
    SwapchainInfo.imageFormat = Format.format;
    SwapchainInfo.imageColorSpace = Format.colorSpace;
    SwapchainInfo.imageExtent = SwapchainExtent;
    SwapchainInfo.imageArrayLayers = 1;
    SwapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VulkanQueueFamilies Indices = FindQueueFamilies(GPU, Surface);

    FUInt32 queueFamilyIndices[] = {
        Indices.GraphicsFamily.value(),
        Indices.PresentFamily.value()
    };

    if (Indices.GraphicsFamily.value() != Indices.PresentFamily.value()) {
        SwapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        SwapchainInfo.queueFamilyIndexCount = 2;
        SwapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        SwapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    SwapchainInfo.preTransform = Support.Capabilities.currentTransform;
    SwapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapchainInfo.presentMode = PresentMode;
    SwapchainInfo.clipped = VK_TRUE;
    SwapchainInfo.oldSwapchain = VK_NULL_HANDLE;
    CHECK_VK_ERR(vkCreateSwapchainKHR(Device, &SwapchainInfo, nullptr, &Swapchain), "Failed to create swapchian");

    Backbuffer = TCreateRef<FVulkanTexture>(this, Swapchain, ImageCount, SwapchainExtent, Format.format);

    VkCommandPoolCreateInfo CommandPoolInfo = {};

    CommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CommandPoolInfo.queueFamilyIndex = GetGraphicsQueueIndex();

    CHECK_VK_ERR(vkCreateCommandPool(Device, &CommandPoolInfo, nullptr, &CommandPool), "Failed to create vulkan command pool");
}

void FVulkanDevice::PresentFrame() {
    VkPresentInfoKHR PresentInfo = {};
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = &Swapchain;
    PresentInfo.waitSemaphoreCount = 1;
    PresentInfo.pWaitSemaphores = &RenderFinishedSemaphores[CurrentFrame];
    PresentInfo.pImageIndices = &ImageIndex;
    VkResult Err = vkQueuePresentKHR(PresentQueue, &PresentInfo);

    if (Err == VK_ERROR_OUT_OF_DATE_KHR || Err == VK_SUBOPTIMAL_KHR || FramebufferResized) {
        FramebufferResized = false;
        RecreateSwapchain();
    } else if (Err != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to present");
    }

    CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void FVulkanDevice::PrepareFrame() {
    vkWaitForFences(Device, 1, &InFlightFences[CurrentFrame], VK_TRUE, UINT64_MAX);

    VkResult Err = vkAcquireNextImageKHR(Device, Swapchain, UINT64_MAX, ImageAvailableSemaphores[CurrentFrame], VK_NULL_HANDLE, &ImageIndex);

    if (Err == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapchain();
        PrepareFrame();
    }
    else if (Err != VK_SUCCESS && Err != VK_SUBOPTIMAL_KHR) {
        SK_LOG_ERROR("Failed to get next swapchain image");
    }

    vkResetFences(Device, 1, &InFlightFences[CurrentFrame]);
}

void FVulkanDevice::WaitForGPUIdle() {
    vkDeviceWaitIdle(Device);
}

void FVulkanDevice::Shutdown() {
    for (VkSemaphore Semaphore : ImageAvailableSemaphores) {
        vkDestroySemaphore(Device, Semaphore, nullptr);
    }

    for (VkSemaphore Semaphore : RenderFinishedSemaphores) {
        vkDestroySemaphore(Device, Semaphore, nullptr);
    }

    for (VkFence Fence : InFlightFences) {
        vkDestroyFence(Device, Fence, nullptr);
    }
    
    vkDestroyCommandPool(Device, CommandPool, nullptr);

    Backbuffer->Shutdown();

    vkDestroySwapchainKHR(Device, Swapchain, nullptr);

    vkDestroyDevice(Device, nullptr);

    vkDestroySurfaceKHR(Instance, Surface, nullptr);

    Initialized = false;
}

void FVulkanDevice::Submit(TRef<IRHICommandContext> CommandContext) {
    // TODO: Allow a command context to have multiple command buffers
    auto Context = std::static_pointer_cast<FVulkanCommandContext>(CommandContext);
    VkCommandBuffer CommandBuffer = Context->GetMainCommandBuffer();

    VkSubmitInfo SubmitInfo = {};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkPipelineStageFlags WaitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    SubmitInfo.waitSemaphoreCount = 1;
    SubmitInfo.pWaitSemaphores = &ImageAvailableSemaphores[CurrentFrame];
    SubmitInfo.pWaitDstStageMask = WaitStages;
    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = &RenderFinishedSemaphores[CurrentFrame];
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBuffer;

    CHECK_VK_ERR(vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, InFlightFences[CurrentFrame]), "Failed to submit to graphics queue");
}

void FVulkanDevice::ImGuiNewFrame() {
    ImGui_ImplVulkan_NewFrame();
}

void FVulkanDevice::ShutdownImGui() {
    ImGui_ImplVulkan_Shutdown();

    vkDestroyDescriptorPool(Device, ImGuiPool, nullptr);
}

void FVulkanDevice::InitImGui() {
    VkDescriptorPoolSize PoolSizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo PoolInfo = {};
    PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    PoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    PoolInfo.maxSets = 1000;
    PoolInfo.poolSizeCount = static_cast<FUInt32>(std::size(PoolSizes));
    PoolInfo.pPoolSizes = PoolSizes;

    CHECK_VK_ERR(vkCreateDescriptorPool(Device, &PoolInfo, nullptr, &ImGuiPool), "Failed to create imgui descriptor pool");

    ImGui_ImplVulkan_InitInfo InitInfo = {};
    InitInfo.Allocator = nullptr;
    InitInfo.CheckVkResultFn = [](VkResult Err) {
        CHECK_VK_ERR(Err, "ImGui Vulkan Error");
    };
    InitInfo.DescriptorPool = ImGuiPool;
    InitInfo.Instance = Instance;
    InitInfo.Device = Device;
    InitInfo.ImageCount = Backbuffer->GetImageCount();
    InitInfo.MinImageCount = Backbuffer->GetImageCount();
    InitInfo.MinAllocationSize = 1024 * 1024;
    InitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    InitInfo.PhysicalDevice = GPU;
    InitInfo.PipelineCache = nullptr;
    VkPipelineRenderingCreateInfo PipelineInfo = {};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    PipelineInfo.viewMask = 0x01;
    PipelineInfo.colorAttachmentCount = 1;
    VkFormat Formats[] = { Backbuffer->GetVulkanFormat()};
    PipelineInfo.pColorAttachmentFormats = Formats;
    PipelineInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
    PipelineInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
    InitInfo.PipelineRenderingCreateInfo = PipelineInfo;
    InitInfo.Queue = GraphicsQueue;
    InitInfo.QueueFamily = GraphicsQueueIndex;
    InitInfo.RenderPass = VK_NULL_HANDLE;
    InitInfo.Subpass = 0;
    InitInfo.UseDynamicRendering = true;

    ImGui_ImplVulkan_Init(&InitInfo);
}

TRef<IRHICommandContext> FVulkanDevice::CreateCommandContext() {
    return TCreateRef<FVulkanCommandContext>(this);
}

FUInt32 FVulkanDevice::GetCurrentFrameIndex() {
    return CurrentFrame;
}

VkDevice FVulkanDevice::GetVulkanDevice() {
    return Device;
}

FUInt32 FVulkanDevice::GetGraphicsQueueIndex() {
    return GraphicsQueueIndex;
}

FUInt32 FVulkanDevice::GetPresentQueueIndex() {
    return PresentQueueIndex;
}

FUInt32 FVulkanDevice::GetCurrentImageIndex() {
    return ImageIndex;
}

TRef<IRHITexture> FVulkanDevice::GetBackbuffer() {
    return Backbuffer;
}

TRef<IRHIBuffer> FVulkanDevice::CreateBuffer(const FRHIBufferDescription &Description) {
    return TCreateRef<FVulkanBuffer>(this, Description);
}

VkCommandBuffer FVulkanDevice::BeginOneTimeVulkanCommandBuffer() {
    VkCommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    AllocInfo.commandPool = CommandPool;
    AllocInfo.commandBufferCount = 1;

    VkCommandBuffer CommandBuffer;
    vkAllocateCommandBuffers(Device, &AllocInfo, &CommandBuffer);

    VkCommandBufferBeginInfo BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(CommandBuffer, &BeginInfo);

    return CommandBuffer;
}

void FVulkanDevice::EndOneTimeVulkanCommandBuffer(VkCommandBuffer CommandBuffer) {
    vkEndCommandBuffer(CommandBuffer);

    VkSubmitInfo SubmitInfo = {};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBuffer;

    vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(GraphicsQueue);

    vkFreeCommandBuffers(Device, CommandPool, 1, &CommandBuffer);
}

void FVulkanDevice::CopyVulkanBuffer(VkBuffer Src, VkBuffer Dst, FUInt32 Size) {
    VkCommandBuffer CommandBuffer = BeginOneTimeVulkanCommandBuffer();

    VkBufferCopy CopyRegion = {};
    CopyRegion.srcOffset = 0;
    CopyRegion.dstOffset = 0;
    CopyRegion.size = Size;

    vkCmdCopyBuffer(CommandBuffer, Src, Dst, 1, &CopyRegion);

    EndOneTimeVulkanCommandBuffer(CommandBuffer);
}

void FVulkanDevice::CreateVulkanBuffer(FUInt32 Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer *Buffer, VkDeviceMemory *Memory) {
    VkBufferCreateInfo BufferInfo = {};
    BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferInfo.size = Size;
    BufferInfo.usage = Usage;
    BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    CHECK_VK_ERR(vkCreateBuffer(Device, &BufferInfo, nullptr, Buffer), "Failed to create buffer");

    VkMemoryRequirements MemReqs = {};
    vkGetBufferMemoryRequirements(Device, *Buffer, &MemReqs);

    VkMemoryAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    AllocInfo.allocationSize = MemReqs.size;
    AllocInfo.memoryTypeIndex = FindVulkanMemoryType(MemReqs.memoryTypeBits, Properties);

    CHECK_VK_ERR(vkAllocateMemory(Device, &AllocInfo, nullptr, Memory), "Failed to allocate memory");
    vkBindBufferMemory(Device, *Buffer, *Memory, 0);
}

FUInt32 FVulkanDevice::FindVulkanMemoryType(FUInt32 Filter, VkMemoryPropertyFlags Flags) {
    VkPhysicalDeviceMemoryProperties MemProps;
    vkGetPhysicalDeviceMemoryProperties(GPU, &MemProps);

    for (FUInt32 MemoryTypeIndex = 0; MemoryTypeIndex < MemProps.memoryTypeCount; MemoryTypeIndex++) {
        if ((Filter & (1 << MemoryTypeIndex)) && ((MemProps.memoryTypes[MemoryTypeIndex].propertyFlags & Flags) == Flags)) {
            return MemoryTypeIndex;
        }
    }

    SK_LOG_ERROR("Failed to get memory type");
    return 0;
}

TRef<IRHIShader> FVulkanDevice::CreateShader(const FRHIShaderDescription &Description) {
    return TCreateRef<FVulkanShader>(this, Description);
}

TRef<IRHIPipelineLayout> FVulkanDevice::CreatePipelineLayout(const FRHIPipelineLayoutDescription &Description) {
    return TCreateRef<FVulkanPipelineLayout>(this, Description);
}

TRef<IRHIDescriptorSetLayout> FVulkanDevice::CreateDescriptorSetLayout(const FRHIDescriptorSetLayoutDescription &Description) {
    return TCreateRef<FVulkanDescriptorSetLayout>(this, Description);
}

TRef<IRHIPipeline> FVulkanDevice::CreatePipeline(const FRHIGraphicsPipelineStateDescription &Description) {
    return TCreateRef<FVulkanPipeline>(this, Description);
}
