#include "OpenGLPlatform.h"

#ifdef SK_OPENGL

#include "Platform.h"
#include "Renderer.h"
#include "Window.h"
#include "Log.h"
#include "Image.h"

/********************
 * Public Interface *
 ********************/

PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLVALIDATEPROGRAMPROC glValidateProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLTEXSTORAGE2DPROC glTexStorage2D;
PFNGLBINDTEXTUREUNITPROC glBindTextureUnit;
PFNGLCREATEBUFFERSPROC glCreateBuffers;
PFNGLNAMEDBUFFERDATAPROC glNamedBufferData;
PFNGLBINDBUFFERBASEPROC glBindBufferBase;
PFNGLNAMEDBUFFERSUBDATAPROC glNamedBufferSubData;
PFNGLCREATEFRAMEBUFFERSPROC glCreateFramebuffers;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;

GLenum
ShaderDataTypeToOpenGLType(shader_data_type Type)
{
  switch (Type)
  {
  case SDT_FLOAT:
    return GL_FLOAT;
  case SDT_FLOAT2:
    return GL_FLOAT;
  case SDT_FLOAT3:
    return GL_FLOAT;
  case SDT_FLOAT4:
    return GL_FLOAT;
  case SDT_MAT3:
    return GL_FLOAT;
  case SDT_MAT4:
    return GL_FLOAT;
  case SDT_INT:
    return GL_INT;
  case SDT_INT2:
    return GL_INT;
  case SDT_INT3:
    return GL_INT;
  case SDT_INT4:
    return GL_INT;
  case SDT_BOOL:
    return GL_BOOL;
  default:
    LogCoreCritical("Unknown Data Type");
    return 0;
  }
}

GLenum
ImageFormatToOpenGLDataFormat(image_format Format)
{
  switch (Format)
  {
  case IMAGE_FORMAT_R8:
    return GL_RED;
  case IMAGE_FORMAT_RGB8:
    return GL_RGB;
  case IMAGE_FORMAT_RGBA8:
    return GL_RGBA;
  case IMAGE_FORMAT_RGBA32F:
    return GL_RGBA;
  default:
    LogCoreError("Invalid Image Format");
    return GL_RGBA;
  }
}

GLenum
ImageFormatToOpenGLInternalFormat(image_format Format)
{
  switch (Format)
  {
  case IMAGE_FORMAT_R8:
    return GL_R8;
  case IMAGE_FORMAT_RGB8:
    return GL_RGB8;
  case IMAGE_FORMAT_RGBA8:
    return GL_RGBA8;
  case IMAGE_FORMAT_RGBA32F:
    return GL_RGBA32F;
  default:
    LogCoreError("Invalid Image Format");
    return GL_RGBA8;
  }
}

#endif
