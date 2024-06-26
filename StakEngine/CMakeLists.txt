file(GLOB sources *.cpp)

add_library(StakEngine STATIC ${sources})

find_package(Vulkan REQUIRED FATAL_ERROR)

target_include_directories(StakEngine PRIVATE External/Vulkan/include)
target_link_libraries(StakEngine PRIVATE ${Vulkan_LIBRARIES})

target_compile_definitions(StakEngine PUBLIC PROJECT_DIR=\"${CMAKE_SOURCE_DIR}\")
