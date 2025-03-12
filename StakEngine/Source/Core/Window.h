#pragma once

#include <functional>

#include "Events.h"
#include "Types.h"
#include "RHIViewport.h"

using FEventFn = std::function<void(FEvent *)>;

struct FWindowConfig {
    FSInt32 Width;
    FSInt32 Height;
    const char *Title;
    FEventFn EventFn;
};

struct FWindowSizeData {
    FSInt32 Width;
    FSInt32 Height;
};

struct FWindowPosData {
    FSInt32 X;
    FSInt32 Y;
};

struct FWindow {
    FSInt32 X;
    FSInt32 Y;
    FSInt32 Width;
    FSInt32 Height;
    FSInt32 FramebufferWidth;
    FSInt32 FramebufferHeight;
    FUInt32 TitleLength;
    const char *Title;
    FEventFn EventFn;
    bool Open;

    // NOTE: Only platform functions should touch this
    //   because it is probably implemented as a pointer/handle
    //   to a platform window
    FHandle PlatformHandle;

    // NOTE: Similarly only renderer/RHI functions should touch this
    TRef<IRHIViewport> Viewport;
};
