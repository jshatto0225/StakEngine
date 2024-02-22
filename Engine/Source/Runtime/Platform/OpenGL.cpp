#include "OpenGL.h"

#include "Platform.h"
#include "Renderer.h"
#include "Window.h"

PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl;
PFNGLCREATEVERTEXARRAYSPROC glCreateVertexArrays;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;

void sk_bind_renderer() {
  glGenBuffers = (PFNGLGENBUFFERSPROC)sk_get_proc_address("glGenBuffers");
  glBindBuffer = (PFNGLBINDBUFFERPROC)sk_get_proc_address("glBindBuffer");
  glBufferData = (PFNGLBUFFERDATAPROC)sk_get_proc_address("glBufferData");
  glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)sk_get_proc_address("glDeleteBuffers");
  glBufferSubData = (PFNGLBUFFERSUBDATAPROC)sk_get_proc_address("glBufferSubData");
  glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)sk_get_proc_address("glDebugMessageCallback");
  glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)sk_get_proc_address("glDebugMessageControl");
  glCreateVertexArrays = (PFNGLCREATEVERTEXARRAYSPROC)sk_get_proc_address("glCreateVertexArrays");
  glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)sk_get_proc_address("glDeleteVertexArrays");
  glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)sk_get_proc_address("glEnableVertexAttribArray");
  glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)sk_get_proc_address("glVertexAttribPointer");
  glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)sk_get_proc_address("glVertexAttribDivisor");
  glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)sk_get_proc_address("glBindVertexArray");
}

void sk_render_command_set_clear_color(f32 r, f32 g, f32 b, f32 a) {
  glClearColor(r, g, b, a);
}

void sk_render_command_clear() {
  glClear(GL_COLOR_BUFFER_BIT);
}

void sk_render_command_init(SKWindow *win) {
  if (!win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  glClearColor(255, 255, 255, 0);
  glViewport(internal_window->x, internal_window->y, internal_window->width, internal_window->height);
}

void sk_render_command_set_viewport(SKWindow *win) {
  if (!win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  glViewport(internal_window->x, internal_window->y, internal_window->width, internal_window->height);
}

void sk_render_command_shutdown() {

}
