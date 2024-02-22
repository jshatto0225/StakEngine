@echo off

REM for /f "delims=" %%A in ('powershell -command "(Get-Location).Path -replace '\\', '/'"') do set "CURRENT_PATH=%%A"
REM echo %CURRENT_PATH%

REM rmdir /s /q .\Build\Debug >nul
REM mkdir .\Build\Debug
REM pushd .\Build\Debug
REM Start-Process vcvars64.bat -Wait
REM cl /EHsc /c /DSK_WINDOWS=1 /DSK_DEBUG=1 /DSOURCE_DIR=\"../../\" /Zi ../../Engine/Source/Runtime/Core/Application.cpp
REM cl /EHsc /c /DSK_WINDOWS=1 /DSK_DEBUG=1 /DSOURCE_DIR=\"../../\" /Zi ../../Engine/Source/Runtime/Platform/Win32/Win32Platform.cpp
REM cl /EHsc /c /DSK_WINDOWS=1 /DSK_DEBUG=1 /DSOURCE_DIR=\"../../\" /Zi ../../Engine/Source/Runtime/Platform/Win32/Win32Window.cpp
REM lib Application.obj Win32Platform.obj Win32Window.obj /OUT:StakRuntime.lib

REM cl /c /Zi /I"../../Engine/Source/" /DSK_DEBUG=1 /DSK_WINDOWS=1 /DSOURCE_DIR=\"%CURRENT_PATH%/\" ../../Engine/Source/Editor/TempMain.cpp
REM link TempMain.obj StakRuntime.lib User32.lib /OUT:StakEditor.exe
REM popd

cmake --preset Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -G"Visual Studio 17 2022"
cmake --build .\Build\
copy Build\Debug\compile_commands.json .\
