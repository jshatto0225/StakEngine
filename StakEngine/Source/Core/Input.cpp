#include "Input.h"

#if defined(SK_WINDOWS) or defined(SK_LINUX)
#include "GLFWInput.h"
#endif

#include "Log.h"

namespace Stak {

Scope<Input> Input::s_Impl;

void Input::init(Ref<Window> window) {
#if defined(SK_WINDOWS) or defined(SK_LINUX)
  s_Impl = createScope<GLFWInput>(window);
#else
  SK_LOG_CRITICAL("Invalid Input Platform");
  s_Impl = NULL;
#endif
}

Input::MousePosData Input::getMousePos() {
  return s_Impl->getMousePosImpl();
}

i32 Input::getMouseX() {
  return getMousePos().x;
}

i32 Input::getMouseY() {
  return getMousePos().y;
}

bool Input::keyDown(KeyCode key) {
  return s_Impl->keyDownImpl(key);
}

bool Input::mouseButtonDown(MouseCode button) {
  return s_Impl->mouseButtonDownImpl(button);
}

} // namespace Stak
