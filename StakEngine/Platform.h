#pragma once

// Set Release Mode
#define SK_DEBUG

#if defined(_WIN32)
    #define SK_WINDOWS
    #define _CRT_SECURE_NO_WARNINGS
#else
    #error Platform not supported
#endif

// NOTE: these will not work if the working dir is not the main proj dir
#define ROOT_DIR PROJECT_DIR "/"
#define ASSET_DIR  PROJECT_DIR "/Assets/"
#define SHADER_DIR PROJECT_DIR "/Assets/Shaders/"
#define IMAGE_DIR  PROJECT_DIR "/Assets/Images/"

typedef void (*proc)();

namespace Platform
{
void Init();
void Shutdown();
} // namespace Platform
