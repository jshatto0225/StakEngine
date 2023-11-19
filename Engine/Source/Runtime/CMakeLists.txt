file(GLOB SOURCES Core/Private/*.cpp  Rendering/Private/*.cpp)

if(WIN32)
    file(GLOB PLATFORM Platform/Win32/Private/*.cpp Platform/Vilkan/Priavte/*.cpp)
    if(VULKAN)
        # Vulkan include and lib paths
        set(Vulkan_LIBRARY C:/VulkanSDK/1.3.268.0/Lib)
        set(Vulkan_INCLUDE_DIR C:/VulkanSDK/1.3.268.0/Include)
    endif()
endif()
    
add_library(StakRuntime STATIC ${SOURCES} ${PLATFORM})

if(VULKAN)
    find_package(Vulkan)
    target_link_libraries(StakRuntime PRIVATE ${Vulkan_LIBRARY}/vulkan-1.lib)
    target_include_directories(StakRuntime PRIVATE ${Vulkan_INCLUDE_DIR})
endif()

target_include_directories(StakRuntime PRIVATE Core/Public)
target_include_directories(StakRuntime PRIVATE Rendering/Public)
target_include_directories(StakRuntime PRIVATE Platform/Vulkan/Public)
target_include_directories(StakRuntime PRIVATE Platform/Win32/Public)

target_compile_definitions(StakRuntime PRIVATE $<$<CONFIG:Debug>: SK_DEBUG=1>)
target_compile_definitions(StakRuntime PRIVATE $<$<CONFIG:Release>: SK_RELEASE=1>)
