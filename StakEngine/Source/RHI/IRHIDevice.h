#pragma once

#include "Types.h"
#include "IWindow.h"
#include "RHICore.h"

class IRHIDevice {
public:
  virtual ~IRHIDevice() = default;

  static TRef<IRHIDevice> Create(TRef<IRHIInstance> Instance, TRef<IWindow> Window);

  virtual TRef<IRHIWorkRecipt> SubmitWork(TRef<IRHIContext> Context) = 0;
  virtual void WaitOnWork(TRef<IRHIWorkRecipt> Recipt) = 0;
  virtual void Present() = 0;

  virtual FUInt32 GetCurrentFrameIndex() const = 0;
  virtual FUInt32 GetCurrentSwapchainImageIndex() const = 0;

  virtual FUInt32 GetSwapchainWidth() const = 0;
  virtual FUInt32 GetSwapchainHeight() const = 0;
  virtual FUInt32 GetSwapchainLayers() const = 0;

  virtual FUInt32 GetMaxFramesInFlight() const = 0;
};
