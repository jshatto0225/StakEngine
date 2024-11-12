#include "Renderer.h"

namespace Stak {

Renderer::Renderer(Ref<Window> window) {
  mRHI = IRHI::create();
  mDevice = mRHI->createDevice(window);
}

Renderer::~Renderer() {
  mDevice = NULL;
  mRHI = NULL;
}

void Renderer::initImGui() {
  mRHI->initImGui(mDevice);
}

void Renderer::imGuiNewFrame() {
  mRHI->imGuiNewFrame();
}

void Renderer::shutdownImGui() {
  mRHI->shutdownImGui();
}

Ref<IRHIBuffer> Renderer::createBuffer(RHIBufferDescription &bufferDesc) {
  return mDevice->createBuffer(bufferDesc);
}

Ref<IRHITexture> Renderer::createTexture(RHITextureDescription &textureDesc) {
  return mDevice->createTexture(textureDesc);
}

Ref<IRHIShader> Renderer::createShader(RHIShaderDescription &shaderDesc) {
  return mDevice->createShader(shaderDesc);
}

Ref<IRHIPipeline> Renderer::createPipeline(RHIPipelineDescription &pipelineDesc) {
  return mDevice->createPipeline(pipelineDesc);
}

Ref<IRHIGraphicsContext> Renderer::createGraphicsContext() {
  return mDevice->createGraphicsContext();
}

void Renderer::processWindowChanges(Ref<Window> window) {
  mDevice->processWindowChanges(window);
}

void Renderer::renderImGuiDrawData(ImDrawData *data, Ref<IRHIGraphicsContext> context) {
  mRHI->renderImGuiDrawData(data, context);
}

} // namespace Stak
