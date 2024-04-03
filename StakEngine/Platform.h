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
#else
  #error Platform not supported
#endif

// NOTE: these will not work if the working dir is not the main proj dir
#define ASSET_DIR  "./Assets/"
#define SHADER_DIR "./Assets/Shaders/"
#define IMAGE_DIR  "./Assets/Images/"

typedef void (*proc)();

void PlatformInit();
void PlatformShutdown();
void PlatformInitExtensions();
void *PlatformGetProcAddress(const char *Name);

void LoadRenderApiLibrary();

void *GetRendererApiProc(const char *Name);
