#include "ImguiLayer.h"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

#if defined(SK_WINDOWS) or defined(SK_LINUX)
#include "VulkanRendererAPI.h"
#endif

#include "Renderer.h"
#include "Log.h"
#include "RendererAPI.h"

namespace Stak {

ImGuiLayer::ImGuiLayer(Ref<Window> window) {
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui::StyleColorsDark();
#if defined(SK_WINDOWS) or defined(SK_LINUX)
  ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow *>(window->getNativeHandle()), true);

  Ref<VulkanRendererAPI> vulkanRenderer = std::dynamic_pointer_cast<VulkanRendererAPI>(Renderer::getAPI());

  ImGui_ImplVulkan_InitInfo initInfo = {};

  initInfo.Instance = vulkanRenderer->getImGuiInstance();
  initInfo.PhysicalDevice = vulkanRenderer->getImGuiPhysicalDevice();
  initInfo.Device = vulkanRenderer->getImGuiDevice();
  initInfo.QueueFamily = vulkanRenderer->getImGuiGraphicsQueueFamily();
  initInfo.Queue = vulkanRenderer->getImGuiGraphicsQueue();
  initInfo.PipelineCache = vulkanRenderer->getImGuiPipelineCache();
  initInfo.DescriptorPool = vulkanRenderer->getImGuiDescriptorPool();
  initInfo.RenderPass = vulkanRenderer->getImGuiRenderPass();
  initInfo.Subpass = 0;
  initInfo.MinImageCount = vulkanRenderer->getImGuiMinImageCount();
  initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  initInfo.Allocator = vulkanRenderer->getImGuiAllocator();
  initInfo.CheckVkResultFn = [](VkResult err) {
    SK_LOG_ERROR("Vulkan ImGui Error");
    };

  ImGui_ImplVulkan_Init(&initInfo);
#endif
}

ImGuiLayer::~ImGuiLayer() {

}

void ImGuiLayer::onAttach() {

}

void ImGuiLayer::onDetach() {

}

void ImGuiLayer::update() {

}

void ImGuiLayer::onEvent(Event &event) {

}

} // namespace Stak
