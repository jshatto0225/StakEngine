#pragma once

#ifdef SK_WINDOWS
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glext.h>

/**
 * OpenGL Fucntions
 * These need to be bound
 */
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
extern PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
extern PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl;
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLVALIDATEPROGRAMPROC glValidateProgram;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLTEXSTORAGE2DPROC glTexStorage2D;
//extern PFNGLACTIVETEXTUREPROC glActiveTexture;
extern PFNGLBINDTEXTUREUNITPROC glBindTextureUnit;
extern PFNGLCREATEBUFFERSPROC glCreateBuffers;
extern PFNGLNAMEDBUFFERDATAPROC glNamedBufferData;
extern PFNGLBINDBUFFERBASEPROC glBindBufferBase;
extern PFNGLNAMEDBUFFERSUBDATAPROC glNamedBufferSubData;
