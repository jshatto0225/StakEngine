#pragma once

#include <GLFW/glfw3.h>

#include "IInput.h"

class FGLFWInput final : public IInput {
public:
  FGLFWInput(TRef<IWindow> window);
  ~FGLFWInput() override;

  FMousePosData GetMousePos() const override;
  FSInt32 GetMouseX() const override;
  FSInt32 GetMouseY() const override;
  bool KeyDown(EKeyCode key) const override;
  bool MouseButtonDown(EMouseCode button) const override;

private:
  TRef<IWindow> mWindow;
};
