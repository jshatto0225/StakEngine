#include "Input.h"

#if defined(SK_WINDOWS) or defined(SK_LINUX)
#include "GLFWInput.h"
#endif

#include "Log.h"

namespace Stak {

Scope<Input> Input::sImpl;

void Input::init(Ref<Window> window) {
#if defined(SK_WINDOWS) or defined(SK_LINUX)
  sImpl = createScope<GLFWInput>(window);
#else
  SK_LOG_CRITICAL("Invalid Input Platform");
  s_Impl = NULL;
#endif
}

void Input::shutdown() {
  sImpl = NULL;
}

Input::MousePosData Input::getMousePos() {
  return sImpl->getMousePosImpl();
}

i32 Input::getMouseX() {
  return getMousePos().x;
}

i32 Input::getMouseY() {
  return getMousePos().y;
}

bool Input::keyDown(KeyCode key) {
  return sImpl->keyDownImpl(key);
}

bool Input::mouseButtonDown(MouseCode button) {
  return sImpl->mouseButtonDownImpl(button);
}

} // namespace Stak
