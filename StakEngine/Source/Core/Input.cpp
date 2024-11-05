#include "Input.h"

#if defined(SK_WINDOWS) or defined(SK_LINUX)
#include "GLFWInput.h"
#endif

#include "Log.h"

namespace Stak {

Scope<Input> Input::s_Impl;

void Input::Init(Ref<Window> window) {
#if defined(SK_WINDOWS) or defined(SK_LINUX)
  s_Impl = CreateScope<GLFWInput>(window);
#else
  SK_LOG_CRITICAL("Invalid Input Platform");
  s_Impl = NULL;
#endif
}

Input::MousePosData Input::GetMousePos() {
  return s_Impl->GetMousePosImpl();
}

i32 Input::GetMouseX() {
  return GetMousePos().X;
}

i32 Input::GetMouseY() {
  return GetMousePos().Y;
}

bool Input::KeyDown(KeyCode key) {
  return s_Impl->KeyDownImpl(key);
}

bool Input::MouseButtonDown(MouseCode button) {
  return s_Impl->MouseButtonDownImpl(button);
}

} // namespace Stak