#include "FGLFWInput.h"

#include "IWindow.h"

#include <GLFW/glfw3.h>

FGLFWInput::FGLFWInput(TRef<IWindow> window) : mWindow(window) {}

FGLFWInput::~FGLFWInput() {
  mWindow = NULL;
}

IInput::FMousePosData FGLFWInput::GetMousePos() const {
  return mWindow->GetMousePos();
}

bool FGLFWInput::KeyDown(EKeyCode Key) const {
  return mWindow->KeyDown(Key);
}

bool FGLFWInput::MouseButtonDown(EMouseCode Button) const {
  return mWindow->MouseButtonDown(Button);
}

FSInt32 FGLFWInput::GetMouseX() const {
  return GetMousePos().X;
}

FSInt32 FGLFWInput::GetMouseY() const {
  return GetMousePos().Y;
}
