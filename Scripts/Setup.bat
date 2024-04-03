@echo off

if not "%1"=="vs2022" if not "%1"=="vs2019" if not "%1"=="vs2017" goto usage
set arg1=%1

echo ************************************************
echo ********** Configuring %1 Project **********
echo ************************************************
.\Scripts\premake5.exe %arg1%
goto :eof

:usage
echo Usage: %0 [vs2022/vs2019/vs2017]