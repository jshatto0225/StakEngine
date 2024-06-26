file(GLOB sources *.cpp)

add_executable(StakEditor ${sources})
target_link_libraries(StakEditor StakEngine)
