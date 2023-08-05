file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AI/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Animation/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Events/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ImGui/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Logging/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ParticleSystem/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Scene/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Scripting/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Utilites/*.cpp

)

file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AI/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Animation/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Events/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ImGui/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Logging/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ParticleSystem/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Scene/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Scripting/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Utilites/*.h
)

add_library(Engine ${SOURCES} ${HEADERS} ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb/stb.cpp)

foreach(header IN LISTS HEADERS)
    get_filename_component(header_path "${header}" DIRECTORY)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/src" "" group_path "${header_path}")
    string(REPLACE "/" "\\" group_name "${group_path}")
    source_group("Header Files${group_name}" FILES "${header}")
endforeach()

foreach(source IN LISTS SOURCES)
    get_filename_component(source_path "${source}" DIRECTORY)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/src" "" group_path "${source_path}")
    string(REPLACE "/" "\\" group_name "${group_path}")
    source_group("Source Files${group_name}" FILES "${source}")
endforeach()

target_link_libraries(Engine PRIVATE 
                      glfw
                      glad
                      imgui)

target_include_directories(Engine PUBLIC
                            ${CMAKE_CURRENT_SOURCE_DIR}/src
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/cereal/include
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/box2d/include
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/lua
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/simpleini
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/EnTT
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/msdf-atlas-gen/msdf-atlas-gen
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/msdf-atlas-gen/msdfgen
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/GLFW/include
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/GLAD/include

        )

target_precompile_headers(Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/stdafx.h)