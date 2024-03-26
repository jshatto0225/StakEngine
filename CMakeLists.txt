cmake_minimum_required(VERSION 3.20)

project(StakEngine)

set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_subdirectory(StakEngine)
add_subdirectory(StakEditor)
#add_subdirectory(Tests)
