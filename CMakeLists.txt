cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
cmake_policy(VERSION 3.8)
project(Cross-Platform-Game-Engine)

set(CMAKE_CXX_STANDARD 17)

source_group("Dependencies" FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
)

include_directories(Engine/vendor
                    Engine/vendor/src
                    Engine/vendor/vendor
                    Engine/vendor/vendor/spdlog/include
                    Engine/vendor/vendor/cereal/include
                    Engine/vendor/vendor/box2d/include
                    Engine/vendor/vendor/lua
                    Engine/vendor/vendor/imgui
                    Engine/vendor/vendor/stb
                    Engine/vendor/vendor/simpleini
                    Engine/vendor/vendor/EnTT
                    Engine/vendor/vendor/msdf-atlas-gen/msdf-atlas-gen
                    Engine/vendor/vendor/msdf-atlas-gen/msdfgen
                    Engine/vendor/vendor/GLFW/include
                    Engine/vendor/vendor/GLAD/include
)

OPTION(BUILD_TESTS "Build the testbed applications" OFF)

find_package(Vulkan)

IF(UNIX AND NOT APPLE)
    set(LINUX TRUE)
ENDIF()

IF(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
ENDIF()

IF (NOT Vulkan_FOUND)
	message(FATAL_ERROR "Could not find Vulkan library!")
ELSE()
	message(STATUS ${Vulkan_LIBRARY})
ENDIF()

add_subdirectory(Engine)
add_subdirectory(Editor)
add_subdirectory(Runtime)

add_subdirectory(Engine/vendor)
add_subdirectory(Engine/vendor/GLFW)