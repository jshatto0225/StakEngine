#pragma once

#include <imgui.h>

#include "RHICommandList.h"
#include "IWindow.h"

class FRenderProxy {
public:
  FRenderProxy();
  virtual ~FRenderProxy();

  virtual void Render(FRHICommandList &CommandList) {}
};


class FRenderer {
public:
  FRenderer();
  ~FRenderer();

  inline static FRenderer &Get() { return *sInstance; }

  void InitImGui();
  void ImGuiNewFrame();
  void ShutdownImGui();
  void Render();

  void AddProxy(FRenderProxy *Proxy);
  void RemoveProxy(FRenderProxy *Proxy);

  void AddPostProxy(FRenderProxy *Proxy);
  void RemovePostProxy(FRenderProxy *Proxy);

  FRenderer(FRenderer &) = delete;
  FRenderer &operator=(FRenderer &) = delete;

private:
  inline static FRenderer *sInstance = NULL;

private:
  std::vector<FRenderProxy *> mRenderProxies;
  std::vector<FRenderProxy *> mPostRenderProxies;
  FRHICommandList mCommandList;
};
