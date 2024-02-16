file(GLOB SOURCES Core/Private/*.cpp  Renderer/Private/*.cpp Platform/Manager/Private/*.cpp)

set(CMAKE_CXX_STANDARD_REQUIRED 20)

if(CMAKE_HOST_WIN32)
  set(SK_WINDOWS 1 GLOBAL)
  set(SK_LINUX 0 GLOBAL)
  set(SK_VULKAN 0 GLOBAL)
  set(SK_OPENGL 1 GLOBAL)

  set(SK_GLFW 0 GLOBAL)
endif()

if(CMAKE_HOST_UNIX)
  set(SK_WINDOWS 0 GLOBAL)
  set(SK_LINUX 1 GLOBAL)
  set(SK_OPENGL 1 GLOBAL)
  set(SK_VULKAN 0 GLOBAL)

  set(SK_GLFW 0 GLOBAL)
endif()

if(SK_WINDOWS EQUAL 1)
  file(GLOB WINDOW_PLATFORM Platform/Win32/Private/*.cpp)
  # Vulkan include and lib paths
endif()

if(SK_VULKAN EQUAL 1)
  file(GLOB RENDERER_PLATFORM Platform/Vulkan/Private/*.cpp)
endif()

if(SK_OPENGL EQUAL 1)
  file(GLOB RENDERER_PLATFORM Platform/OpenGL/Private/*.cpp)
endif()

if(SK_LINUX EQUAL 1)
  file(GLOB WINDOW_PLATFORM Platform/Linux/Private/*.cpp)
endif()

if(SK_GLFW EQUAL 1)
  file(GLOB WINDOW_PLATFORM Platform/GLFW/Private/*.cpp)
  add_subdirectory(External/GLFW)
endif()

add_library(StakRuntime STATIC ${SOURCES} ${WINDOW_PLATFORM} ${RENDERER_PLATFORM})

if(SK_WINDOWS EQUAL 1)
  target_compile_definitions(StakRuntime PRIVATE SK_WINDOWS=1)
  target_include_directories(StakRuntime PRIVATE Platform/Win32/Public)
  target_link_libraries(StakRuntime PRIVATE user32.lib)
endif()

if(SK_LINUX EQUAL 1)
  target_compile_definitions(StakRuntime PRIVATE SK_LINUX=1)
  target_include_directories(StakRuntime PRIVATE Platform/Linux/Public)
  target_link_libraries(StakRuntime PRIVATE X11)
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
  target_link_libraries(StakRuntime PUBLIC ${OPENGL_LIBRARIES})
  target_include_directories(StakRuntime PRIVATE ${OPENGL_INCLUDE_DIRS})
  target_include_directories(StakRuntime PRIVATE Platform/OpenGL/Public)
endif()

if(SK_GLFW EQUAL 1)
  target_compile_definitions(StakRuntime PRIVATE SK_GLFW=1)
  target_include_directories(StakRuntime PRIVATE External/GLFW/include)
  target_include_directories(StakRuntime PRIVATE Platform/GLFW/Public)
  target_link_libraries(StakRuntime PUBLIC glfw3)
endif()

target_include_directories(StakRuntime PUBLIC Core/Public)
target_include_directories(StakRuntime PUBLIC Renderer/Public)
target_include_directories(StakRuntime PUBLIC Math/Vector/Public)
target_include_directories(StakRuntime PUBLIC Math/Matrix/Public)
target_include_directories(StakRuntime PUBLIC Math/Public)
target_include_directories(StakRuntime PUBLIC Platform/Manager/Public)

target_compile_definitions(StakRuntime PUBLIC $<$<CONFIG:Debug>: SK_DEBUG=1>)
target_compile_definitions(StakRuntime PUBLIC $<$<CONFIG:Release>: SK_RELEASE=1>)
target_compile_definitions(StakRuntime PUBLIC "SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")
