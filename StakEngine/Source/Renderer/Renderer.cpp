#include "Renderer.h"

namespace Stak {

Renderer::Renderer(Ref<Window> window) {
  mRHI = IRHI::create();
  mDevice = mRHI->createDevice(window);
}

Renderer::~Renderer() {
}

void Renderer::initImGui() {
  Ref<IRHIGraphicsContext> context = mDevice->createGraphicsContext();
  mRHI->initImGui(mDevice, context);
}

void Renderer::imGuiNewFrame() {
  mRHI->imGuiNewFrame();
}

void Renderer::processWindowChanges(Ref<Window> window) {
  mDevice->processWindowChanges(window);
}

} // namespace Stak
