file(GLOB SOURCES Core/Private/*.cpp  Renderer/Private/*.cpp Platform/Manager/Private/*.cpp)

set(CMAKE_CXX_STANDARD_REQUIRED 20)

if(WIN32) 
    set(SK_WINDOWS 1 GLOBAL)
    set(SK_VULKAN 0 GLOBAL)
    set(SK_OPENGL 1 GLOBAL)
endif()

if(WIN32)
    file(GLOB WINDOW_PLATFORM Platform/Win32/Private/*.cpp)
    # Vulkan include and lib paths
    if(SK_VULKAN EQUAL 1)
        #set(Vulkan_LIBRARY C:/VulkanSDK/1.3.268.0/Lib)
        #set(Vulkan_INCLUDE_DIR C:/VulkanSDK/1.3.268.0/Include)
        file(GLOB RENDERER_PLATFORM Platform/Vulkan/Private/*.cpp)
    endif()
    if(SK_OPENGL EQUAL 1)
        file(GLOB RENDERER_PLATFORM Platform/OpenGL/Private/*.cpp)
    endif()
endif()

add_library(StakRuntime STATIC ${SOURCES} ${WINDOW_PLATFORM} ${RENDERER_PLATFORM})


if(WIN32)
    target_compile_definitions(StakRuntime PRIVATE SK_WINDOWS=1)
    target_include_directories(StakRuntime PRIVATE Platform/Win32/Public)
    target_link_libraries(StakRuntime PRIVATE user32.lib)
endif()

if(SK_VULKAN EQUAL 1)
    target_compile_definitions(StakRuntime PRIVATE SK_VULKAN=1)
    find_package(Vulkan)
    target_link_libraries(StakRuntime PRIVATE ${Vulkan_LIBRARY})
    target_include_directories(StakRuntime PRIVATE ${Vulkan_INCLUDE_DIR})
    target_include_directories(StakRuntime PRIVATE Platform/Vulkan/Public)
endif()

if(SK_OPENGL EQUAL 1)
    target_compile_definitions(StakRuntime PRIVATE SK_OPENGL=1)
    find_package(OpenGL REQUIRED)
    target_link_libraries(StakRuntime PRIVATE ${OPENGL_LIBRARIES})
    target_include_directories(StakRuntime PRIVATE ${OPENGL_INCLUDE_DIRS})
    target_include_directories(StakRuntime PRIVATE Platform/OpenGL/Public)
endif()

target_include_directories(StakRuntime PUBLIC Core/Public)
target_include_directories(StakRuntime PUBLIC Renderer/Public)
target_include_directories(StakRuntime PUBLIC Platform/Manager/Public)

target_compile_definitions(StakRuntime PUBLIC $<$<CONFIG:Debug>: SK_DEBUG=1>)
target_compile_definitions(StakRuntime PUBLIC $<$<CONFIG:Release>: SK_RELEASE=1>)
