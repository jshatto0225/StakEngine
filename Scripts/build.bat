@echo off

if not exist "build" (
  mkdir build
  echo Directory created: build
)

echo Starting Build

set GLMPath=..\Engine\Source\Runtime\External\glm\
set RuntimePath=..\Engine\Source\Runtime
set RuntimeCorePath=%RuntimePath%\Core
set RuntimeAssetLoadingPath=%RuntimePath%\AssetLoading
set RuntimeMathPath=%RuntimePath%\Math
set RuntimePlatformPath=%RuntimePath%\Platform
set RuntimeRendererPath=%RuntimePath%\Renderer
set RuntimeScenePath=%RuntimePath%\Scene

set RuntimeCore=%RuntimeCorePath%\Application.cpp %RuntimeCorePath%\Log.cpp
set RuntimeMath=%RuntimeMathPath%\StakMath.cpp
set RuntimePlatform= %RuntimePlatformPath%\OpenGL.cpp %RuntimePlatformPath%\WGL.cpp %RuntimePlatformPath%\Win32.cpp
set RuntimeRenderer=%RuntimeRendererPath%\Renderer.cpp %RuntimeRendererPath%\Camera.cpp
set RuntimeScene=%RuntimeScenePath%\Registry.cpp

set RuntimeSource=%RuntimeCore% %RuntimeMath% %RuntimePlatform% %RuntimeRenderer% %RuntimeScene%
set AllInclude=/I%GLMPath% /I%RuntimeCorePath% /I%RuntimeAssetLoadingPath% /I%RuntimeMathPath% /I%RuntimePlatformPath% /I%RuntimeRendererPath% /I%RuntimeScenePath%
set RuntimeFlags=/Zi /c /EHsc

set dir=%~dp0
set SourceDir^=\"%dir:\=/%../Engine/Source/\"
set ShaderDir^=\"%dir:\=/%../Engine/Assets/Shaders/\"
set AssetDir^=\"%dir:\=/%../Engine/Assets/\"
set AllMacros=/DSOURCE_DIR=%SourceDir% /DSHADER_DIR=%ShaderDir% /DASSET_DIR=%AssetDir% /DSK_WINDOWS /DSK_WGL /DSK_DEBUG

set RuntimeLibSource=Application.obj Camera.obj Log.obj OpenGL.obj Registry.obj Renderer.obj StakMath.obj WGL.obj Win32.obj

set EditorPath=..\Engine\Source\Editor
set EditorCorePath=%EditorPath%\Core
set EditorRendererPath=%EditorPath%\Renderer

set EditorSource=%EditorCorePath%\Editor.cpp %EditorCorePath%\EditorLayer.cpp
set EditorInclude=/I..\Engine\Source
set EditorFlags=/Zi /EHsc

pushd build
echo %SourceDir%
cl %RuntimeFlags% %RuntimeSource% %AllInclude% %AllMacros%
lib %RuntimeLibSource% user32.lib opengl32.lib Gdi32.lib Kernel32.lib /OUT:StakRuntime.lib
cl %AllMacros% %EditorSource% %EditorInclude% %AllInclude%  %EditorFlags% StakRuntime.lib /FeStakEditor.exe

popd
