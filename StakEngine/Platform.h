#pragma once

#define SK_DEBUG

#define SK_OPENGL

#if defined(WIN32) or defined(_WIN32)
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

#define ASSET_DIR "C:/Users/jungl/dev/StakEngineReleaseEnv/Assets/"
#define SHADER_DIR "C:/Users/jungl/dev/StakEngineReleaseEnv/Assets/Shaders/"
#define IMAGE_DIR "C:/Users/jungl/dev/StakEngineReleaseEnv/Assets/Images/"

typedef void (*proc)();

void PlatformInit();
void PlatformShutdown();
void PlatformInitExtensions();
void *PlatformGetProcAddress(const char *Name);

void LoadRenderApiLibrary();

void *GetRendererApiProc(const char *Name);
