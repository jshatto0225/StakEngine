file(GLOB SOURCES Core/Private/*.cpp Platform/Win32/Private/*.cpp)

add_library(StakRuntime STATIC ${SOURCES})

target_include_directories(StakRuntime PRIVATE Core/Public)
target_include_directories(StakRuntime PRIVATE Platform/Win32/Public)

target_compile_definitions(StakRuntime PRIVATE $<$<CONFIG:Debug>: DEBUG=1>)
target_compile_definitions(StakRuntime PRIVATE $<$<CONFIG:Release>: RELEASE=1>)