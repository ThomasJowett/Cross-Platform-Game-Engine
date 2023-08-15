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

if(APPLE)
    set(EDITOR_ICON ${CMAKE_CURRENT_SOURCE_DIR}/Icon.icns)
    set_source_files_properties(${EDITOR_ICON} PROPERTIES 
        MACOSX_PACKAGE_LOCATION "Resources")
    add_executable(Editor MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${EDITOR_ICON})

    set_target_properties(Editor PROPERTIES 
        BUNDLE True
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.Jowett.Thomas.Editor"
        MACOSX_BUNDLE_BUNDLE_NAME "Editor"
        MACOSX_BUNDLE_BUNDLE_VERSION "0.0.1"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "0.0.1"
    )
else()
    add_executable(Editor ${SOURCES} ${HEADERS} Icon.ico ${RESOURCE_FILES})
endif()

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