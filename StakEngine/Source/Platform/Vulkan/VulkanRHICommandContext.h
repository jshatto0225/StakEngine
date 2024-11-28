#pragma once

#include "RHICommandContext.h"
#include "VulkanRHI.h"
#include "FLog.h"

class FVulkanRHICommandContext final : public IRHICommandContext {
public:

  inline void BeginDrawing() override {
    mIsActive = true;
    FRHI::Get().BeginDrawing();
  }

  inline void EndDrawing() override {
    mIsActive = false;
    FRHI::Get().EndDrawing();
  }

  inline void ResourceTransition(IRHITransition *Transition) override {
    SK_LOG_ERROR("Resource transitions not implemented");
  }

  inline void RenderImGuiDrawData(ImDrawData *DrawData) override {
    FRHI::Get().SubmitImGuiDrawData(DrawData);
  }

  inline FBool IsActive() override { return mIsActive; }

private:
  FBool mIsActive = false;
};