#pragma once

#include "Platform.h"

#ifdef SK_OPENGL

#ifdef SK_WINDOWS
#include <windows.h>
#undef CreateWindow
#undef DestroyWindow
#define GLAPIENTRYP __stdcall *
#endif

#include <GL/gl.h>

#include "ShaderDataTypes.h"
#include "Types.h"
#include "Image.h"
#include "External/GL/glext.h"

extern PFNGLGENBUFFERSPROC Stak_glGenBuffers;
#define glGenBuffers Stak_glGenBuffers
extern PFNGLCREATEFRAMEBUFFERSPROC Stak_glCreateFramebuffers;
#define glCreateFramebuffers Stak_glCreateFramebuffers
extern PFNGLDELETEFRAMEBUFFERSPROC Stak_glDeleteFramebuffers;
#define glDeleteFramebuffers Stak_glDeleteFramebuffers
extern PFNGLBINDFRAMEBUFFERPROC Stak_glBindFramebuffer;
#define glBindFramebuffer Stak_glBindFramebuffer
extern PFNGLFRAMEBUFFERTEXTURE2DPROC Stak_glFramebufferTexture2D;
#define glFramebufferTexture2D Stak_glFramebufferTexture2D
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC Stak_glCheckFramebufferStatus;
#define glCheckFramebufferStatus Stak_glCheckFramebufferStatus
extern PFNGLBINDBUFFERPROC Stak_glBindBuffer;
#define glBindBuffer Stak_glBindBuffer
extern PFNGLBUFFERDATAPROC Stak_glBufferData;
#define glBufferData Stak_glBufferData
extern PFNGLDELETEBUFFERSPROC Stak_glDeleteBuffers;
#define glDeleteBuffers Stak_glDeleteBuffers
extern PFNGLBUFFERSUBDATAPROC Stak_glBufferSubData;
#define glBufferSubData Stak_glBufferSubData
extern PFNGLDEBUGMESSAGECALLBACKPROC Stak_glDebugMessageCallback;
#define glDebugMessageCallback Stak_glDebugMessageCallback
extern PFNGLDEBUGMESSAGECONTROLPROC Stak_glDebugMessageControl;
#define glDebugMessageControl Stak_glDebugMessageControl
extern PFNGLGENVERTEXARRAYSPROC Stak_glGenVertexArrays;
#define glGenVertexArrays Stak_glGenVertexArrays
extern PFNGLDELETEVERTEXARRAYSPROC Stak_glDeleteVertexArrays;
#define glDeleteVertexArrays Stak_glDeleteVertexArrays
extern PFNGLENABLEVERTEXATTRIBARRAYPROC Stak_glEnableVertexAttribArray;
#define glEnableVertexAttribArray Stak_glEnableVertexAttribArray
extern PFNGLVERTEXATTRIBPOINTERPROC Stak_glVertexAttribPointer;
#define glVertexAttribPointer Stak_glVertexAttribPointer
extern PFNGLVERTEXATTRIBDIVISORPROC Stak_glVertexAttribDivisor;
#define glVertexAttribDivisor Stak_glVertexAttribDivisor
extern PFNGLBINDVERTEXARRAYPROC Stak_glBindVertexArray;
#define glBindVertexArray Stak_glBindVertexArray
extern PFNGLCREATEPROGRAMPROC Stak_glCreateProgram;
#define glCreateProgram Stak_glCreateProgram
extern PFNGLCREATESHADERPROC Stak_glCreateShader;
#define glCreateShader Stak_glCreateShader
extern PFNGLSHADERSOURCEPROC Stak_glShaderSource;
#define glShaderSource Stak_glShaderSource
extern PFNGLCOMPILESHADERPROC Stak_glCompileShader;
#define glCompileShader Stak_glCompileShader
extern PFNGLATTACHSHADERPROC Stak_glAttachShader;
#define glAttachShader Stak_glAttachShader
extern PFNGLLINKPROGRAMPROC Stak_glLinkProgram;
#define glLinkProgram Stak_glLinkProgram
extern PFNGLVALIDATEPROGRAMPROC Stak_glValidateProgram;
#define glValidateProgram Stak_glValidateProgram
extern PFNGLDELETESHADERPROC Stak_glDeleteShader;
#define glDeleteShader Stak_glDeleteShader
extern PFNGLDELETEPROGRAMPROC Stak_glDeleteProgram;
#define glDeleteProgram Stak_glDeleteProgram
extern PFNGLUSEPROGRAMPROC Stak_glUseProgram;
#define glUseProgram Stak_glUseProgram
extern PFNGLGETSHADERIVPROC Stak_glGetShaderiv;
#define glGetShaderiv Stak_glGetShaderiv
extern PFNGLGETPROGRAMIVPROC Stak_glGetProgramiv;
#define glGetProgramiv Stak_glGetProgramiv
extern PFNGLGETSHADERINFOLOGPROC Stak_glGetShaderInfoLog;
#define glGetShaderInfoLog Stak_glGetShaderInfoLog
extern PFNGLGETPROGRAMINFOLOGPROC Stak_glGetProgramInfoLog;
#define glGetProgramInfoLog Stak_glGetProgramInfoLog
extern PFNGLGETUNIFORMLOCATIONPROC Stak_glGetUniformLocation;
#define glGetUniformLocation Stak_glGetUniformLocation
extern PFNGLUNIFORM1IPROC Stak_glUniform1i;
#define glUniform1i Stak_glUniform1i
extern PFNGLTEXSTORAGE2DPROC Stak_glTexStorage2D;
#define glTexStorage2D Stak_glTexStorage2D
extern PFNGLBINDTEXTUREUNITPROC Stak_glBindTextureUnit;
#define glBindTextureUnit Stak_glBindTextureUnit
extern PFNGLCREATEBUFFERSPROC Stak_glCreateBuffers;
#define glCreateBuffers Stak_glCreateBuffers
extern PFNGLNAMEDBUFFERDATAPROC Stak_glNamedBufferData;
#define glNamedBufferData Stak_glNamedBufferData
extern PFNGLBINDBUFFERBASEPROC Stak_glBindBufferBase;
#define glBindBufferBase Stak_glBindBufferBase
extern PFNGLNAMEDBUFFERSUBDATAPROC Stak_glNamedBufferSubData;
#define glNamedBufferSubData Stak_glNamedBufferSubData

typedef const GLubyte *(GLAPIENTRYP GLGETSTRINGPROC)(GLenum);
extern GLGETSTRINGPROC Stak_glGetString;
#define glGetString Stak_glGetString
typedef void (GLAPIENTRYP GLBINDTEXTUREPROC)(GLenum, GLuint);
extern GLBINDTEXTUREPROC Stak_glBindTexture;
#define glBindTexture Stak_glBindTexture
typedef void (GLAPIENTRYP GLBLENDFUNCPROC)(GLenum, GLenum);
extern GLBLENDFUNCPROC Stak_glBlendFunc;
#define glBlendFunc Stak_glBlendFunc
typedef void (GLAPIENTRYP GLCLEARPROC)(GLbitfield);
extern GLCLEARPROC Stak_glClear;
#define glClear Stak_glClear
typedef void (GLAPIENTRYP GLCLEARCOLORPROC)(GLfloat, GLfloat, GLfloat, GLfloat);
extern GLCLEARCOLORPROC Stak_glClearColor;
#define glClearColor Stak_glClearColor
typedef void (GLAPIENTRYP GLDELETETEXTURESPROC)(GLsizei, const GLuint *);
extern GLDELETETEXTURESPROC Stak_glDeleteTextures;
#define glDeleteTextures Stak_glDeleteTextures
typedef void (GLAPIENTRYP GLDRAWELEMENTSPROC)(GLenum, GLsizei, GLenum, const void *);
extern GLDRAWELEMENTSPROC Stak_glDrawElements;
#define glDrawElements Stak_glDrawElements
typedef void (GLAPIENTRYP GLENABLEPROC)(GLenum);
extern GLENABLEPROC Stak_glEnable;
#define glEnable Stak_glEnable
typedef void (GLAPIENTRYP GLGENTRXTURESPROC)(GLsizei, GLuint *);
extern GLGENTRXTURESPROC Stak_glGenTextures;
#define glGenTextures Stak_glGenTextures
typedef void (GLAPIENTRYP GLTEXPARAMETERIPROC)(GLenum, GLenum, GLint);
extern GLTEXPARAMETERIPROC Stak_glTexParameteri;
#define glTexParameteri Stak_glTexParameteri
typedef void (GLAPIENTRYP GLTEXSUBIMAGE2DPROC)(GLenum target,
                                              GLint level,
                                              GLint xoffset,
                                              GLint yoffset,
                                              GLsizei width,
                                              GLsizei height,
                                              GLenum format,
                                              GLenum type,
                                              const void *pixels);
extern GLTEXSUBIMAGE2DPROC Stak_glTexSubImage2D;
#define glTexSubImage2D Stak_glTexSubImage2D
typedef void (GLAPIENTRYP GLVIEWPORTPROC)(GLint, GLint, GLsizei, GLsizei);
extern GLVIEWPORTPROC Stak_glViewport;
#define glViewport Stak_glViewport
typedef void (GLAPIENTRYP GLTEXIMAGE2DPROC)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *);
extern GLTEXIMAGE2DPROC Stak_glTexImage2D;
#define glTexImage2D Stak_glTexImage2D

GLenum ShaderDataTypeToOpenGLType(shader_data_type Type);
GLenum ImageFormatToOpenGLDataFormat(image_format Format);
GLenum ImageFormatToOpenGLInternalFormat(image_format Format);

#endif