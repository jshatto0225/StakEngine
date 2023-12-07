cmake_minimum_required(VERSION 3.24)

project(StakEngine)

set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(Engine/Source/Runtime)
add_subdirectory(Engine/Source/Editor)
add_subdirectory(Tests)
