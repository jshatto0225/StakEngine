file(GLOB SOURCES *.cpp)

add_executable(UnitTests ${SOURCES})

#target_link_libraries(UnitTests StakRuntime)

target_include_directories(UnitTests PRIVATE ${CMAKE_SOURCE_DIR}/Engine/Source/Runtime/Core/Public)
target_include_directories(UnitTests PRIVATE ${CMAKE_SOURCE_DIR}/Engine/Source/Runtime/Core/Private)
target_include_directories(UnitTests PRIVATE ${CMAKE_SOURCE_DIR}/Engine/Source/Runtime/Platform/Win32/Public)
target_include_directories(UnitTests PRIVATE ${CMAKE_SOURCE_DIR}/Engine/Source/Runtime/Platform/Win32/Private)
target_include_directories(UnitTests PRIVATE ${CMAKE_SOURCE_DIR}/Engine/Source/Runtime/Platform/Vulkan/Public)
target_include_directories(UnitTests PRIVATE ${CMAKE_SOURCE_DIR}/Engine/Source/Runtime/Platform/Vulkan/Private)
target_include_directories(UnitTests PRIVATE ${CMAKE_SOURCE_DIR}/Engine/Source/Runtime/Renderer/Public)
target_include_directories(UnitTests PRIVATE ${CMAKE_SOURCE_DIR}/Engine/Source/Runtime/Renderer/Private)

target_compile_definitions(UnitTests PRIVATE SK_DEBUG)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
