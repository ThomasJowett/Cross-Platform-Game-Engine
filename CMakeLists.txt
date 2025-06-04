cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
cmake_policy(VERSION 3.8)
project(Cross-Platform-Game-Engine)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE "Debug")

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

find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS})

IF(UNIX AND NOT APPLE)
    set(LINUX TRUE)
ENDIF()

IF(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
ENDIF()

if (MSVC)
    add_compile_options(/MP /ZI /JMC)
    set(DEFAULT_STARTUP_TARGET Editor)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${DEFAULT_STARTUP_TARGET})
endif()

add_subdirectory(Engine)
add_subdirectory(Editor)
add_subdirectory(Runtime)

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
# set(GLFW_LIBRARY_TYPE OFF CACHE INTERNAL "")
set(tinyxml2_BUILD_TESTING OFF CACHE INTERNAL "")
set(tinyxml2_INSTALL_CMAKEDIR OFF CACHE INTERNAL "")
set(tinyxml2_INSTALL_PKGCONFIGDIR OFF CACHE INTERNAL "")

add_subdirectory(Engine/vendor)
add_subdirectory(Editor/vendor)
add_subdirectory(Engine/vendor/GLFW)
add_subdirectory(Engine/vendor/TinyXml2)
add_subdirectory(Engine/vendor/box2d)
add_subdirectory(Engine/vendor/tracy)

set(THIRD_PARTY_FOLDER "ThirdParty")

set_target_properties(glfw PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/GLFW)
set_target_properties(update_mappings PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/GLFW)
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
set_target_properties(miniaudio PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
set_target_properties(TracyClient PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})


