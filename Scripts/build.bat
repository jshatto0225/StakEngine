@echo off

if not exist "Build" (
  mkdir Build
  echo Directory created: Build
)

echo Starting Build

set EnginePath=..\StakEngine
set EngineFiles=%EnginePath%\Application.cpp %EnginePath%\Log.cpp %EnginePath%\StakMath.cpp %EnginePath%\OpenGL.cpp %EnginePath%\WGL.cpp %EnginePath%\Win32.cpp %EnginePath%\Renderer.cpp %EnginePath%\Camera.cpp %EnginePath%\Renderer2D.cpp
set EngineFlags=/Zi /c

set EngineLib=Application.obj Camera.obj Log.obj OpenGL.obj Renderer.obj StakMath.obj WGL.obj Win32.obj Renderer2D.obj

set EditorPath=..\StakEditor

set EditorSource=%EditorPath%\Editor.cpp %EditorPath%\EditorLayer.cpp
set EditorFlags=/Zi /EHsc

pushd build
cl %EngineFlags% %EngineFiles%
lib %EngineLib% /OUT:StakRuntime.lib
cl %EditorSource% %EditorFlags% StakRuntime.lib /FeStakEditor.exe

popd
