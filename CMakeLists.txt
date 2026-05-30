cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
cmake_policy(VERSION 3.10)
project(Cross-Platform-Game-Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (EMSCRIPTEN)
    add_link_options("-sUSE_GLFW=3" "-sUSE_WEBGPU=1" "-sASYNCIFY" "-sALLOW_MEMORY_GROWTH=1" "-sNO_EXIT_RUNTIME=1" "-sASSERTIONS=1")
endif()

option(ENABLE_UNITY_BUILD "Enable unity build" OFF)
if(ENABLE_UNITY_BUILD)
    set(CMAKE_UNITY_BUILD ON)
endif()

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

function(group_files_by_directory FILE_LIST)
    foreach(file IN LISTS ${FILE_LIST})
        get_filename_component(file_path "${file}" DIRECTORY)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/src" "" group_path "${file_path}")
        string(REPLACE "/" "\\" group_name "${group_path}")
        source_group("${group_name}" FILES "${file}")
    endforeach()
endfunction()

include_directories(Engine/vendor
Engine/src
Engine/vendor
Engine/vendor/spdlog/include
Engine/vendor/cereal/include
Engine/vendor/box2d/include
Engine/vendor/lua
Engine/vendor/imgui
Engine/vendor/stb
Engine/vendor/simpleini
Engine/vendor/EnTT
Engine/vendor/msdf-atlas-gen/msdf-atlas-gen
Engine/vendor/msdf-atlas-gen/msdfgen
Engine/vendor/GLFW/include
Engine/vendor/GLAD/include
)

if (NOT EMSCRIPTEN)
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIRS})
endif()


IF(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
    set(LINUX TRUE)
ENDIF()

if (MSVC)
    add_compile_options(/MP /ZI /JMC)
    set(DEFAULT_STARTUP_TARGET Editor)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${DEFAULT_STARTUP_TARGET})
endif()

set(BOX2D_USER_SETTINGS OFF CACHE INTERNAL "")
set(BOX2D_BUILD_TESTBED OFF CACHE INTERNAL "")
set(BOX2D_BUILD_DOCS OFF CACHE INTERNAL "")
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE INTERNAL "")
set(BUILD_TESTING OFF CACHE INTERNAL "")
# set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
set(GLFW_INSTALL OFF CACHE INTERNAL "" )
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
if (LINUX)
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
elseif (EMSCRIPTEN)
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
endif()
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
# set(GLFW_LIBRARY_TYPE OFF CACHE INTERNAL "")
set(tinyxml2_BUILD_TESTING OFF CACHE INTERNAL "")
set(tinyxml2_INSTALL_CMAKEDIR OFF CACHE INTERNAL "")
set(tinyxml2_INSTALL_PKGCONFIGDIR OFF CACHE INTERNAL "")

add_subdirectory(Engine/vendor)
add_subdirectory(Editor/vendor)
if (NOT EMSCRIPTEN)
    add_subdirectory(Engine/vendor/GLFW)
endif()
add_subdirectory(Engine/vendor/TinyXml2)
add_subdirectory(Engine/vendor/box2d)
add_subdirectory(Engine/vendor/glfw3webgpu)
add_subdirectory(Engine/vendor/tracy)
add_subdirectory(Engine/vendor/sol)
add_subdirectory(Engine/vendor/WebGPU-distribution)
add_subdirectory(Engine/vendor/miniz)

add_subdirectory(Engine)
add_subdirectory(Editor)
add_subdirectory(Runtime)

set(THIRD_PARTY_FOLDER "ThirdParty")
if (TARGET glfw)
    set_target_properties(glfw PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/GLFW)
endif()
if (TARGET update_mappings)
    set_target_properties(update_mappings PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/GLFW)
endif()
set_target_properties(tinyxml2 PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
set_target_properties(box2d PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
set_target_properties(freetype PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
set_target_properties(glad PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
set_target_properties(imgui PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
set_target_properties(lua PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
set_target_properties(miniz PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
set_target_properties(msdf-atlas-gen PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/msdfgen)
set_target_properties(msdfgen PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/msdfgen)
set_target_properties(OpenFBX PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
set_target_properties(libdeflate PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
set_target_properties(miniaudio PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
set_target_properties(TracyClient PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
set_target_properties(glfw3webgpu PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/GLFW)


