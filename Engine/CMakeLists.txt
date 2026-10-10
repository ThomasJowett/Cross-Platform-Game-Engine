file(GLOB_RECURSE ENGINE_FILES
    src/AI/*.cpp
    src/Animation/*.cpp
    src/Asset/*.cpp
    src/Core/*.cpp
    src/Events/*.cpp
    src/Logging/*.cpp
    src/Particles/*.cpp
    src/Physics/*.cpp
    src/Platform/Null/*.cpp
    src/Platform/OpenGL/*.cpp
    src/Platform/WebGPU/*.cpp
    src/Renderer/*.cpp
    src/Scene/*.cpp
    src/Scripting/*.cpp
    src/Utilities/*.cpp
    src/*.h
    src/*.hpp
    src/ImGui/ImGuiBuildGlfw.cpp
    src/ImGui/ImGuiManager.cpp
    src/ImGui/ImGuiUtilities.cpp
)

add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>" "$<$<CONFIG:DEBUG>:-DENABLE_ASSERTS>")

add_library(Engine ${ENGINE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb/stb.cpp)

group_files_by_directory("ENGINE_FILES")

target_link_libraries(Engine PRIVATE
                      ${OpenGL_LIBRARY}
                      glfw
                      glad
                      imgui
                      tinyxml2
                      lua
                      box2d
                      msdf-atlas-gen
                      miniaudio
                      miniz
                      webgpu
                      glfw3webgpu
                      )

target_link_libraries(Engine PUBLIC miniz)

add_definitions(
    -DGLFW_INCLUDE_NONE
    -D_CRT_SECURE_NO_WARNINGS
)

target_compile_definitions(Engine PUBLIC UNICODE _UNICODE)
# sol2 type checks in every build; PUBLIC so all targets share one sol configuration
target_compile_definitions(Engine PUBLIC SOL_ALL_SAFETIES_ON=1)

if (MSVC)
    target_compile_options(Engine PRIVATE "/bigobj" "/utf-8")
else ()
  add_compile_options(-Wa,-mbig-obj)
endif ()

if(XCODE)
    set_target_properties(Engine PROPERTIES
        XCODE_GENERATE_SCHEME ON
        XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal")
endif()

target_include_directories(Engine PUBLIC
                            src
                            vendor
                            vendor/GLFW/include
                            vendor/GLFW/deps
                            vendor/GLAD/include
                            vendor/spdlog/include
                            vendor/cereal/include
                            vendor/box2d/include
                            vendor/lua
                            vendor/imgui
                            vendor/stb
                            vendor/simpleini
                            vendor/EnTT
                            vendor/msdf-atlas-gen/msdf-atlas-gen
                            vendor/msdf-atlas-gen/msdfgen
                            vendor/sol/include
        )

target_precompile_headers(Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/stdafx.h)

set(ENABLE_PROFILING OFF CACHE BOOL "Enable Profiling with Tracy")

if(ENABLE_PROFILING)
    target_compile_definitions(Engine PUBLIC ATCP_ENABLE_TRACY)
    target_include_directories(Engine PUBLIC "${CMAKE_SOURCE_DIR}/Engine/vendor/tracy/public/tracy")
    target_link_libraries(Engine PUBLIC Tracy::TracyClient)
    message("enabling profiling")
endif()