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

if(WIN32)
    file(GLOB DIRECTX_FILES
        src/ImGui/ImGuiBuildDirectX11.cpp
        src/Platform/DirectX/DirectX11Buffer.cpp
        src/Platform/DirectX/DirectX11Buffer.h
        src/Platform/DirectX/DirectX11FrameBuffer.cpp
        src/Platform/DirectX/DirectX11FrameBuffer.h
        src/Platform/DirectX/DirectX11RendererAPI.cpp
        src/Platform/DirectX/DirectX11RendererAPI.h
        src/Platform/DirectX/DirectX11Texture.cpp
        src/Platform/DirectX/DirectX11Texture.h
        src/Platform/DirectX/DirectX11Context.cpp
        src/Platform/DirectX/DirectX11Context.h
        src/Platform/DirectX/DirectX11Pipeline.cpp
        src/Platform/DirectX/DirectX11Pipeline.h
        src/Platform/DirectX/DirectX11Shader.cpp
        src/Platform/DirectX/DirectX11Shader.h
        src/Platform/DirectX/DirectX11UniformBuffer.cpp
        src/Platform/DirectX/DirectX11UniformBuffer.h
    )
endif()

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

add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>" "$<$<CONFIG:DEBUG>:-DENABLE_ASSERTS>")

add_library(Engine ${ENGINE_FILES} ${DIRECTX_FILES} ${OPENGL_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb/stb.cpp)

group_files_by_directory("ENGINE_FILES")
group_files_by_directory("DIRECTX_FILES")
group_files_by_directory("OPENGL_FILES")

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