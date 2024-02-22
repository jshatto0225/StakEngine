#include "OpenGLRendererAPI.h"

#include <algorithm>

#include "Log.h"
#include "OpenGLBindings.h"
#include "PlatformManager.h"
#include "Renderer2D.h"
#include "Types.h"

void APIENTRY opengl_message_callback(u32 source, u32 type, u32 id, u32 severity, i32 length, const char *message, const void *userParam) {
  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH: {
    SK_CORE_CRITICAL(message);
    break;
  }
  case GL_DEBUG_SEVERITY_MEDIUM: {
    SK_CORE_ERROR(message);
    break;
  }
  case GL_DEBUG_SEVERITY_LOW: {
    SK_CORE_WARN(message);
    break;
  }
  case GL_DEBUG_SEVERITY_NOTIFICATION: {
    SK_CORE_TRACE(message);
    break;
  }
  }
}

OpenGLRendererAPI::OpenGLRendererAPI(Window *window) {
  context = sk_create_context(window);

#ifdef SK_DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(opengl_message_callback, nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
}

void OpenGLRendererAPI::draw_indexed(VertexArray *vao, u32 count) {
  vao->bind();
  u32 newCount = count ? count : vao->get_index_buffer()->get_count();
  glDrawElements(GL_TRIANGLES, newCount, GL_UNSIGNED_INT, nullptr);
}

void OpenGLRendererAPI::draw_lines(VertexArray *vao, u32 count) {
  vao->bind();
  glDrawArrays(GL_LINES, 0, count);
}

void OpenGLRendererAPI::set_line_width(f32 width) {
  glLineWidth(width);
}

void OpenGLRendererAPI::clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::set_clear_color(const Vec4f &color) {
  glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRendererAPI::set_viewport(const Vec4u &viewport) {
  glViewport(viewport(0), viewport(1), viewport(2), viewport(3));
}

