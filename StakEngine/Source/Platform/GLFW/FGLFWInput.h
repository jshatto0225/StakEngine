#pragma once

#include <GLFW/glfw3.h>

#include "IInput.h"

class FGLFWInput final : public IInput {
public:
  FGLFWInput(TRef<IWindow> Window);
  ~FGLFWInput() override;

  FMousePosData GetMousePos() const override;
  FSInt32 GetMouseX() const override;
  FSInt32 GetMouseY() const override;
  bool KeyDown(EKeyCode Key) const override;
  bool MouseButtonDown(EMouseCode Button) const override;

private:
  TRef<IWindow> mWindow;
};
