#include "Window.h"

#include "Application.h"
#include "Event.h"
#include "Log.h"

namespace sk {
u64 Window::current_id = 0;

Window::Window() {
  if (current_id > UINT_MAX) {
    SK_CORE_WARN("Used significant number of unique windows. Id Overflow is "
                 "not checked.");
  }
  id = ++current_id;
}

void Window::generate_event(EventType e) {
  switch (e) {
  case WINDOW_CLOSE: {
    WindowCloseEvent event(get_id());
    Application::on_event(event);
  } break;
  case WINDOW_RECT_CHANGED: {
    WindowRectChangedEvent event(get_id(), get_width(), get_height(), get_x(), get_y());
    Application::on_event(event);
  } break;
  default:
    break;
  }
}

u64 Window::get_id() { return id; }
} // namespace SK
