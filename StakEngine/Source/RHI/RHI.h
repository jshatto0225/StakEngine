#pragma once

#include "Types.h"
#include "RHIForward.h"
#include "Window.h"

#include <imgui.h>

class IRHI {
public:
    IRHI() = default;

    IRHI(IRHI &) = delete;
    IRHI &operator=(IRHI &) = delete;

public:
    virtual TRef<IRHIDevice> CreateDevice(TRef<IWindow> Window) = 0;
};

void RHIInit();
void RHIShutdown();
TRef<IRHIDevice> RHICreateDeviceImpl(TRef<IWindow> Window);
TRef<IRHIDevice> RHICreateDevice(TRef<IWindow> Window);