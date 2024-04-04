#pragma once

// Set Release Mode
#define SK_DEBUG

// Set Renderer API
#define SK_OPENGL

#if defined(_WIN32)
  #define SK_WINDOWS
  #define _CRT_SECURE_NO_WARNINGS
  #ifdef SK_OPENGL
    #define SK_WGL
  #else
    #error Renderer not supported
  #endif
#elif defined(__linux__)
  #define SK_LINUX

  // TODO:
  #define SK_X11

  #if defined(SK_OPENGL) and defined(SK_X11)
    #define SK_GLX
  #endif
#else
  #error Platform not supported
#endif

#ifndef PROJECT_DIR
#error Project direcory not defined
#endif

// NOTE: these will not work if the working dir is not the main proj dir
#define ROOT_DIR PROJECT_DIR "/"
#define ASSET_DIR  PROJECT_DIR "/Assets/"
#define SHADER_DIR PROJECT_DIR "/Assets/Shaders/"
#define IMAGE_DIR  PROJECT_DIR "/Assets/Images/"

typedef void (*proc)();

void PlatformInit();
void PlatformShutdown();
void PlatformInitExtensions();
void *PlatformGetProcAddress(const char *Name);

void LoadRenderApiLibrary();

void *GetRendererApiProc(const char *Name);
