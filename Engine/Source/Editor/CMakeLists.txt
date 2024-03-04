add_executable(StakEditor "Core/Editor.cpp"  "Core/EditorLayer.cpp" "Renderer/UIRenderer.h")

target_link_libraries(StakEditor StakRuntime)

target_include_directories(StakEditor PRIVATE ${CMAKE_SOURCE_DIR}/Engine/Source)
target_include_directories(StakEditor PRIVATE Core)
