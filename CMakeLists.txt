cmake_minimum_required(VERSION 3.24)

project(StakEngine)

add_subdirectory(Engine/Source/Runtime)
add_subdirectory(Engine/Source/Editor)
add_subdirectory(Tests)

set(CMAKE_CXX_STANDARD_REQUIRED 20)
