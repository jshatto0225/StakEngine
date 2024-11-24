#pragma once

#include "Types.h"
#include "RHICore.h"

#include <string>
#include <imgui.h>

class IRHIInstance {
public:
  virtual ~IRHIInstance() = default;

  static TScope<IRHIInstance> Create(const std::string &AppName);

  virtual void InitImGui(TRef<IRHIDevice> Device) = 0;
  virtual void ShutdownImGui(TRef<IRHIDevice> Device) = 0;
  virtual void ImGuiNewFrame() = 0;
  virtual void SubmitImGuiDrawData(ImDrawData *DrawData, TRef<IRHIGraphicsContext> Context) = 0;
};
