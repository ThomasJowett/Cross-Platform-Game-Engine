file(GLOB SOURCES
    src/*.cpp
    src/FileSystem/*.cpp
    src/Fonts/*.cpp
    src/History/*.cpp
    src/ImGui/*.cpp
    "src/ImGui/Node Editor/*.cpp"
    src/Importers/*.cpp
    src/Interfaces/*.cpp
    src/Panels/*.cpp
    src/Styles/*.cpp
    src/Toolbars/*.cpp
    src/Viewers/*.cpp
)

file(GLOB HEADERS
    src/*.h
    src/*.h
    src/FileSystem/*.h
    src/Fonts/*.h
    src/History/*.h
    src/ImGui/*.h
    "src/ImGui/Node Editor/*.h"
    src/Importers/*.h
    src/Interfaces/*.h
    src/Panels/*.h
    src/Styles/*.h
    src/Toolbars/*.h
    src/Viewers/*.h
)

if(MSVC)
    file(GLOB RESOURCE_FILES
        resource.h
        Editor.rc
    )
endif()

add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>" "$<$<CONFIG:DEBUG>:-DENABLE_ASSERTS>")

add_executable(Editor ${SOURCES} ${HEADERS} Icon.ico ${RESOURCE_FILES})

group_files_by_directory("SOURCES")
group_files_by_directory("HEADERS")

target_link_libraries(Editor PRIVATE Engine OpenFBX)

target_include_directories(Editor PUBLIC
                            ${CMAKE_CURRENT_SOURCE_DIR}/src
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/IconFont
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/OpenFBX/src
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/json
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/tinygltf
        )

add_definitions(
    -D_CRT_SECURE_NO_WARNINGS
    -DUNICODE
)

add_custom_command(
    TARGET Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/data"
    "$<TARGET_FILE_DIR:Editor>/data"
)

add_custom_command(
    TARGET Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui.ini
    "$<TARGET_FILE_DIR:Editor>"
)