#pragma once

#include "Input.h"

#include <GLFW/glfw3.h>

void GLFWEnableRawInput(TRef<IWindow> Window);
void GLFWDisableRawInput(TRef<IWindow> Window);
void GLFWSetCursorVisibility(TRef<IWindow> Window, ECursorVisibility Visibility);
