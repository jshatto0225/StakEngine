#pragma once

#include "Types.h"
#include "RHIForward.h"
#include "Window.h"

#include <imgui.h>

class FRHI {
public:
    FRHI() = default;

    FRHI(FRHI &) = delete;
    FRHI &operator=(FRHI &) = delete;

public:
    virtual TRef<IRHIDevice> CreateDevice(TRef<IWindow> Window) = 0;
};

void RHIInit();
void RHIShutdown();
TRef<IRHIDevice> RHICreateDeviceImpl(TRef<IWindow> Window);
FRHIDevice RHICreateDevice(TRef<IWindow> Window);