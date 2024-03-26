add_executable(StakEditor "Editor.cpp"  "EditorLayer.cpp")

target_link_libraries(StakEditor StakEngine)

target_include_directories(StakEditor PRIVATE ${CMAKE_SOURCE_DIR})
