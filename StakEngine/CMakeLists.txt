file(GLOB SOURCES *.cpp)

set(CMAKE_CXX_STANDARD_REQUIRED 20)

add_library(StakEngine STATIC ${SOURCES} ${WINDOW_PLATFORM} ${RENDERER_PLATFORM})

target_compile_definitions(StakEngine PUBLIC "SOURCE_DIR=\"${CMAKE_SOURCE_DIR}/\"")
target_compile_definitions(StakEngine PUBLIC "ASSET_DIR=\"${CMAKE_SOURCE_DIR}/Assets/\"")
target_compile_definitions(StakEngine PUBLIC "SHADER_DIR=\"${CMAKE_SOURCE_DIR}/Assets/Shaders/\"")
target_compile_definitions(StakEngine PUBLIC "IMAGE_DIR=\"${CMAKE_SOURCE_DIR}/Assets/Images/\"")