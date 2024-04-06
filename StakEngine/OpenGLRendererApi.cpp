#include "OpenGLPlatform.h"

#ifdef SK_OPENGL

#include "RenderApi.h"
#include "Log.h"

/*********************
 * Private Interface *
 *********************/

bool RenderApiInitialized = false;
window *RenderApiWindow = NULL;

static void APIENTRY
OpenGLMessageCallback(u32 Source, u32 Type, u32 Id, u32 Severity, i32 Length, const char *Message, const void *UserParam)
{
  switch (Severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    LogCoreCritical(Message);
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    LogCoreError(Message);
    break;
  case GL_DEBUG_SEVERITY_LOW:
    LogCoreWarn(Message);
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    LogCoreTrace(Message);
    break;
  }
}

/********************
 * Public Interface *
 ********************/

void
RenderApiInit(window *Window)
{
  if (RenderApiInitialized)
  {
    return;
  }

#pragma warning(disable : 4067)
#if defined(SK_DEBUG) or defined(SK_RELEASE)
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(OpenGLMessageCallback, NULL);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
                        GL_DEBUG_SEVERITY_NOTIFICATION,
                        0,
                        NULL,
                        GL_FALSE);
#endif

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);

  RenderApiWindow = Window;

  RenderApiInitialized = true;
}

void
RenderApiShutdown()
{
  RenderApiInitialized = false;
}

void
RenderApiBind()
{
  const unsigned char *Version = glGetString(GL_VERSION);
  LogCoreTrace("OpenGL Version %s", Version);

  // Other Functions
  glBindTextureUnit = (PFNGLBINDTEXTUREUNITPROC)PlatformGetProcAddress("glBindTextureUnit");
  glGenBuffers = (PFNGLGENBUFFERSPROC)PlatformGetProcAddress("glGenBuffers");
  glBindBuffer = (PFNGLBINDBUFFERPROC)PlatformGetProcAddress("glBindBuffer");
  glBufferData = (PFNGLBUFFERDATAPROC)PlatformGetProcAddress("glBufferData");
  glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)PlatformGetProcAddress("glDeleteBuffers");
  glBufferSubData = (PFNGLBUFFERSUBDATAPROC)PlatformGetProcAddress("glBufferSubData");
  glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)PlatformGetProcAddress("glDebugMessageCallback");
  glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)PlatformGetProcAddress("glDebugMessageControl");
  glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)PlatformGetProcAddress("glGenVertexArrays");
  glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)PlatformGetProcAddress("glDeleteVertexArrays");
  glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)PlatformGetProcAddress("glEnableVertexAttribArray");
  glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)PlatformGetProcAddress("glVertexAttribPointer");
  glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)PlatformGetProcAddress("glVertexAttribDivisor");
  glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)PlatformGetProcAddress("glBindVertexArray");
  glCreateProgram = (PFNGLCREATEPROGRAMPROC)PlatformGetProcAddress("glCreateProgram");
  glCreateShader = (PFNGLCREATESHADERPROC)PlatformGetProcAddress("glCreateShader");
  glShaderSource = (PFNGLSHADERSOURCEPROC)PlatformGetProcAddress("glShaderSource");
  glCompileShader = (PFNGLCOMPILESHADERPROC)PlatformGetProcAddress("glCompileShader");
  glAttachShader = (PFNGLATTACHSHADERPROC)PlatformGetProcAddress("glAttachShader");
  glLinkProgram = (PFNGLLINKPROGRAMPROC)PlatformGetProcAddress("glLinkProgram");
  glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)PlatformGetProcAddress("glValidateProgram");
  glDeleteShader = (PFNGLDELETESHADERPROC)PlatformGetProcAddress("glDeleteShader");
  glDeleteProgram = (PFNGLDELETEPROGRAMPROC)PlatformGetProcAddress("glDeleteProgram");
  glUseProgram = (PFNGLUSEPROGRAMPROC)PlatformGetProcAddress("glUseProgram");
  glGetShaderiv = (PFNGLGETSHADERIVPROC)PlatformGetProcAddress("glGetShaderiv");
  glGetProgramiv = (PFNGLGETPROGRAMIVPROC)PlatformGetProcAddress("glGetProgramiv");
  glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)PlatformGetProcAddress("glGetShaderInfoLog");
  glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)PlatformGetProcAddress("glGetProgramInfoLog");
  glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)PlatformGetProcAddress("glGetUniformLocation");
  glUniform1i = (PFNGLUNIFORM1IPROC)PlatformGetProcAddress("glUniform1i");
  glTexStorage2D = (PFNGLTEXSTORAGE2DPROC)PlatformGetProcAddress("glTexStorage2D");
  glCreateBuffers = (PFNGLCREATEBUFFERSPROC)PlatformGetProcAddress("glCreateBuffers");
  glNamedBufferData = (PFNGLNAMEDBUFFERDATAPROC)PlatformGetProcAddress("glNamedBufferData");
  glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)PlatformGetProcAddress("glBindBufferBase");
  glNamedBufferSubData = (PFNGLNAMEDBUFFERSUBDATAPROC)PlatformGetProcAddress("glNamedBufferSubData");
  glCreateFramebuffers = (PFNGLCREATEFRAMEBUFFERSPROC)PlatformGetProcAddress("glCreateFramebuffers");
  glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)PlatformGetProcAddress("glDeleteFramebuffers");
  glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)PlatformGetProcAddress("glBindFramebuffer");
  glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)PlatformGetProcAddress("glFramebufferTexture2D");
  glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)PlatformGetProcAddress("glCheckFramebufferStatus");
}

void
RenderApiSwapBuffers()
{
  SwapWindowBuffers(RenderApiWindow);
}

void
RenderApiSetClearColor(f32 Red, f32 Green, f32 Blue, f32 Alpha)
{
  glClearColor(Red, Green, Blue, Alpha);
}

void
RenderApiClear()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void
RenderApiSetViewport(i32 X, i32 Y, i32 Width, i32 Height)
{
  glViewport(X, Y, Width, Height);
}

void
RenderApiSetLineWidth(f32 Width)
{
  // TODO:
}

void
RenderApiDrawIndexed(const vertex_array *VertexArray, u32 Count)
{
  if (Count == 0)
  {
    Count = GetIndexBufferIndexCount(GetVertexArrayIndexBuffer(VertexArray));
  }
  glDrawElements(GL_TRIANGLES, Count, GL_UNSIGNED_INT, NULL);
}

void
RenderApiDrawLines(const vertex_array *VertexArray, u32 Count)
{
  //TODO:
}

#endif
