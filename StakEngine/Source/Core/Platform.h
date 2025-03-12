#pragma once

#include "Window.h"

bool PlatformInit();
void PlatformShutdown();

bool PlatformOpenWindow(FWindow *Window, const FWindowConfig *Cfg);
void PlatformCloseWindow(FWindow *Window);

bool PlatformInitImGui(const FWindow *Window);

void PlatformImGuiNewFrame();
void PlatformShutdownImGui();
void PlatformProcessMessages();

void PlatformEnableRawInput(const FWindow *Window);
void PlatformDisableRawInput(const FWindow *Window);
void PlatformSetCursorVisibility(const FWindow *Window, ECursorVisibility Visibility);