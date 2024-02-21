file(GLOB SOURCES Core/*.cpp)

set(CMAKE_CXX_STANDARD_REQUIRED 20)

if(CMAKE_HOST_WIN32)
  set(SK_WINDOWS 1 GLOBAL)
  set(SK_LINUX 0 GLOBAL)
  set(SK_VULKAN 0 GLOBAL)
  set(SK_OPENGL 1 GLOBAL)
endif()

if(CMAKE_HOST_UNIX)
  set(SK_WINDOWS 0 GLOBAL)
  set(SK_LINUX 1 GLOBAL)
  set(SK_OPENGL 1 GLOBAL)
  set(SK_VULKAN 0 GLOBAL)
endif()

if(SK_WINDOWS EQUAL 1)
  file(GLOB WINDOW_PLATFORM Platform/Win32/*.cpp)
  # Vulkan include and lib paths
endif()

#if(SK_VULKAN EQUAL 1)
#  file(GLOB RENDERER_PLATFORM Platform/Vulkan/*.cpp)
#endif()

#if(SK_OPENGL EQUAL 1)
#  file(GLOB RENDERER_PLATFORM Platform/OpenGL/*.cpp)
#endif()

#if(SK_LINUX EQUAL 1)
#  file(GLOB WINDOW_PLATFORM Platform/Linux/*.cpp)
#endif()

add_library(StakRuntime STATIC ${SOURCES} ${WINDOW_PLATFORM} ${RENDERER_PLATFORM})

if(SK_WINDOWS EQUAL 1)
  target_compile_definitions(StakRuntime PRIVATE SK_WINDOWS=1)
  target_include_directories(StakRuntime PRIVATE Platform/Win32)
  target_link_libraries(StakRuntime PRIVATE user32.lib)
endif()

#if(SK_LINUX EQUAL 1)
#  target_compile_definitions(StakRuntime PRIVATE SK_LINUX=1)
#  target_include_directories(StakRuntime PRIVATE Platform/Linux)
#  target_link_libraries(StakRuntime PRIVATE X11)
#endif()

#if(SK_VULKAN EQUAL 1)
#  target_compile_definitions(StakRuntime PRIVATE SK_VULKAN=1)
#  find_package(Vulkan)
#  target_link_libraries(StakRuntime PRIVATE ${Vulkan_LIBRARY})
#  target_include_directories(StakRuntime PRIVATE ${Vulkan_INCLUDE_DIR})
#  target_include_directories(StakRuntime PRIVATE Platform/Vulkan)
#endif()

#if(SK_OPENGL EQUAL 1)
#  target_compile_definitions(StakRuntime PRIVATE SK_OPENGL=1)
#  find_package(OpenGL REQUIRED)
#  target_link_libraries(StakRuntime PUBLIC ${OPENGL_LIBRARIES})
#  target_include_directories(StakRuntime PRIVATE ${OPENGL_INCLUDE_DIRS})
#  target_include_directories(StakRuntime PRIVATE Platform/OpenGL)
#endif()

target_include_directories(StakRuntime PRIVATE Core)
target_include_directories(StakRuntime PRIVATE Renderer)
target_include_directories(StakRuntime PRIVATE Math/Vector)
target_include_directories(StakRuntime PRIVATE Math/Matrix)
target_include_directories(StakRuntime PRIVATE Platform/Manager)
target_include_directories(StakRuntime PRIVATE Platform)

target_compile_definitions(StakRuntime PUBLIC $<$<CONFIG:Debug>: SK_DEBUG=1>)
target_compile_definitions(StakRuntime PUBLIC $<$<CONFIG:Release>: SK_RELEASE=1>)
target_compile_definitions(StakRuntime PUBLIC "SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")
