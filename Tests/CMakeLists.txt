file(GLOB SOURCES *.cpp)

add_executable(UnitTests ${SOURCES})

target_link_libraries(UnitTests StakRuntime)

target_include_directories(UnitTests PRIVATE ${CMAKE_SOURCE_DIR}/Engine/Source)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
