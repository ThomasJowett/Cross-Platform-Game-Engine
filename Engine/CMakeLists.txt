file(GLOB_RECURSE ENGINE_FILES
    src/AI/*.cpp
    src/Animation/*.cpp
    src/Asset/*.cpp
    src/Core/*.cpp
    src/Events/*.cpp
    src/Logging/*.cpp
    src/Particles/*.cpp
    src/Physics/*.cpp
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

file (GLOB OPENGL_FILES
    src/Platform/OpenGL/OpenGLBuffer.cpp
    src/Platform/OpenGL/OpenGLBuffer.h
    src/Platform/OpenGL/OpenGLFrameBuffer.cpp
    src/Platform/OpenGL/OpenGLFrameBuffer.h
    src/Platform/OpenGL/OpenGLRendererAPI.cpp
    src/Platform/OpenGL/OpenGLRendererAPI.h
    src/Platform/OpenGL/OpenGLTexture.cpp
    src/Platform/OpenGL/OpenGLTexture.h
    src/Platform/OpenGL/OpenGLVertexArray.cpp
    src/Platform/OpenGL/OpenGLVertexArray.h
    src/Platform/OpenGL/OpenGLContext.cpp
    src/Platform/OpenGL/OpenGLContext.h
    src/Platform/OpenGL/OpenGLPipeline.cpp
    src/Platform/OpenGL/OpenGLPipeline.h
    src/Platform/OpenGL/OpenGLShader.cpp
    src/Platform/OpenGL/OpenGLShader.h
    src/Platform/OpenGL/OpenGLUniformBuffer.cpp
    src/Platform/OpenGL/OpenGLUniformBuffer.h
)

file (GLOB WEBGPU_FILES
    src/Platform/WebGPU/WebGPU.cpp
    src/Platform/WebGPU/WebGPUBuffer.cpp
    src/Platform/WebGPU/WebGPUBuffer.h
    src/Platform/WebGPU/WebGPUFrameBuffer.cpp
    src/Platform/WebGPU/WebGPUFrameBuffer.h
    src/Platform/WebGPU/WebGPURendererAPI.cpp
    src/Platform/WebGPU/WebGPURendererAPI.h
    src/Platform/WebGPU/WebGPUTexture.cpp
    src/Platform/WebGPU/WebGPUTexture.h
    src/Platform/WebGPU/WebGPUVertexArray.cpp
    src/Platform/WebGPU/WebGPUVertexArray.h
    src/Platform/WebGPU/WebGPUContext.cpp
    src/Platform/WebGPU/WebGPUContext.h
    src/Platform/WebGPU/WebGPUPipeline.cpp
    src/Platform/WebGPU/WebGPUPipeline.h
    src/Platform/WebGPU/WebGPUShader.cpp
    src/Platform/WebGPU/WebGPUShader.h
    src/Platform/WebGPU/WebGPUUniformBuffer.cpp
    src/Platform/WebGPU/WebGPUUniformBuffer.h
)

add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>" "$<$<CONFIG:DEBUG>:-DENABLE_ASSERTS>")

add_library(Engine ${ENGINE_FILES} ${OPENGL_FILES} ${WEBGPU_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb/stb.cpp)

group_files_by_directory("ENGINE_FILES")
group_files_by_directory("OPENGL_FILES")
group_files_by_directory("WEBGPU_FILES")

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

add_definitions(
    -DGLFW_INCLUDE_NONE
    -D_CRT_SECURE_NO_WARNINGS
    -DUNICODE
)

if (MSVC)
    target_compile_options(Engine PRIVATE "/bigobj")
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
                            ${CMAKE_SOURCE_DIR}/Editor/vendor/OpenFBX/src
        )

target_precompile_headers(Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/stdafx.h)

set(ENABLE_PROFILING OFF CACHE BOOL "Enable Profiling with Tracy")

if(ENABLE_PROFILING)
    add_compile_definitions(ATCP_ENABLE_TRACY)
    target_include_directories(Engine PUBLIC "${CMAKE_SOURCE_DIR}/Engine/vendor/tracy/public/tracy")
    target_link_libraries(Engine PUBLIC Tracy::TracyClient)
    message("enabling profiling")
endif()