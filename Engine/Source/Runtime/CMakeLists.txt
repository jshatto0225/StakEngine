file(GLOB SOURCES Core/Private/*.cpp  Renderer/Private/*.cpp Platform/Manager/Private/*.cpp)

if(WIN32)
    file(GLOB WINDOW_PLATFORM Platform/Win32/Private/*.cpp)
    # Vulkan include and lib paths
    set(Vulkan_LIBRARY C:/VulkanSDK/1.3.268.0/Lib)
    set(Vulkan_INCLUDE_DIR C:/VulkanSDK/1.3.268.0/Include)
    file(GLOB RENDERER_PLATFORM Platform/Vulkan/Private/*.cpp)
endif()

add_library(StakRuntime STATIC ${SOURCES} ${WINDOW_PLATFORM} ${RENDERER_PLATFORM})


if(WIN32)
    target_compile_definitions(StakRuntime PRIVATE VULKAN=1)
    find_package(Vulkan)
    target_link_libraries(StakRuntime PRIVATE ${Vulkan_LIBRARY}/vulkan-1.lib)
    target_include_directories(StakRuntime PRIVATE ${Vulkan_INCLUDE_DIR})
    target_include_directories(StakRuntime PRIVATE Platform/Vulkan/Public)
    target_include_directories(StakRuntime PRIVATE Platform/Win32/Public)
    target_link_libraries(StakRuntime PRIVATE user32.lib)
endif()

target_include_directories(StakRuntime PUBLIC Core/Public)
target_include_directories(StakRuntime PUBLIC Renderer/Public)
target_include_directories(StakRuntime PUBLIC Platform/Manager/Public)

target_compile_definitions(StakRuntime PRIVATE $<$<CONFIG:Debug>: SK_DEBUG=1>)
target_compile_definitions(StakRuntime PRIVATE $<$<CONFIG:Release>: SK_RELEASE=1>)
