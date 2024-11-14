#pragma once

#include "Types.h"
#include "IWindow.h"
#include "RHICore.h"

class IRHIDevice {
public:
  virtual ~IRHIDevice() = default;

  static TRef<IRHIDevice> Create(TRef<IRHIInstance> Instance, TRef<IWindow> Window);
};
