add_executable(StakEditor "TempMain.cpp")

target_link_libraries(StakEditor StakRuntime)

target_include_directories(StakEditor PRIVATE ${CMAKE_SOURCE_DIR}/Engine/Source)

set(VULKAN 1)