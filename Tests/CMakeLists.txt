file(GLOB SOURCES *.cpp)

set(CMAKE_CXX_STANDARD_REQUIRED 20)

add_executable(UnitTests UnitTests.cpp)

target_link_libraries(UnitTests StakRuntime)

target_include_directories(UnitTests PRIVATE ${CMAKE_SOURCE_DIR}/Engine/Source)
