#include "OpenGL.h"

#include "Platform.h"
#include "Renderer.h"
#include "Window.h"
#include "Log.h"

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

void RenderApi::Bind() {
  glGenBuffers = (PFNGLGENBUFFERSPROC)Platform::GetProcAddress("glGenBuffers");
  glBindBuffer = (PFNGLBINDBUFFERPROC)Platform::GetProcAddress("glBindBuffer");
  glBufferData = (PFNGLBUFFERDATAPROC)Platform::GetProcAddress("glBufferData");
  glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)Platform::GetProcAddress("glDeleteBuffers");
  glBufferSubData = (PFNGLBUFFERSUBDATAPROC)Platform::GetProcAddress("glBufferSubData");
  glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)Platform::GetProcAddress("glDebugMessageCallback");
  glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)Platform::GetProcAddress("glDebugMessageControl");
  glCreateVertexArrays = (PFNGLCREATEVERTEXARRAYSPROC)Platform::GetProcAddress("glCreateVertexArrays");
  glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)Platform::GetProcAddress("glDeleteVertexArrays");
  glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)Platform::GetProcAddress("glEnableVertexAttribArray");
  glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)Platform::GetProcAddress("glVertexAttribPointer");
  glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)Platform::GetProcAddress("glVertexAttribDivisor");
  glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)Platform::GetProcAddress("glBindVertexArray");
}

void APIENTRY OpenGLMessageCallback(u32 source, u32 type, u32 id, u32 severity, i32 length, const char *message, const void *user_param) {
  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    Log::CoreCritical(message);
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    Log::CoreError(message);
    break;
  case GL_DEBUG_SEVERITY_LOW:
    Log::CoreWarn(message);
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    Log::CoreTrace(message);
    break;
  }
}

bool RenderApi::initialized = false;

void RenderApi::Init() {
  if (initialized) {
    return;
  }

#if defined(SK_DEBUG) or defined(SK_RELEASE)
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(OpenGLMessageCallback, nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);

  initialized = true;
}

void RenderApi::Shutdown() {
  initialized = false;
}

void RenderApi::SetClearColor(f32 r, f32 g, f32 b, f32 a) {
  glClearColor(r, g, b, a);
}

void RenderApi::Clear() {
  glClear(GL_COLOR_BUFFER_BIT);
}

void RenderApi::SetViewPort(i32 x, i32 y, i32 width, i32 height) {
  glViewport(x, y, width, height);
}
