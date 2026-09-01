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

file(GLOB_RECURSE DATA_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/data/*")

if(APPLE)
    file(GLOB_RECURSE MAC_SOURCES "src/*.mm")
    list(APPEND SOURCES ${MAC_SOURCES})

    set_source_files_properties(${MAC_SOURCES} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
    set_source_files_properties(${MAC_SOURCES} PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)

    set(EDITOR_ICON ${CMAKE_CURRENT_SOURCE_DIR}/Icon.icns)
    set_source_files_properties(${EDITOR_ICON} PROPERTIES 
        MACOSX_PACKAGE_LOCATION "Resources")

    foreach(file ${DATA_FILES})
        file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_SOURCE_DIR}/data" ${file})
        get_filename_component(rel_dir "${rel_path}" DIRECTORY)
        set_source_files_properties(${file} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources/data/${rel_dir}")
    endforeach()

    add_executable(Editor MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${EDITOR_ICON} ${DATA_FILES})

    set_target_properties(Editor PROPERTIES 
        BUNDLE True
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.Jowett.Thomas.Editor"
        MACOSX_BUNDLE_BUNDLE_NAME "Editor"
        MACOSX_BUNDLE_BUNDLE_VERSION "0.0.1"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "0.0.1"
        XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "entitlements.plist"
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "@executable_path"
    )

    add_custom_command(TARGET Editor POST_BUILD
        COMMAND codesign --entitlements "${CMAKE_CURRENT_SOURCE_DIR}/entitlements.plist" --deep -f -s "-" "$<TARGET_BUNDLE_DIR:Editor>"
        COMMENT "Codesigning Editor bundle with entitlements"
    )
else()
    add_executable(Editor ${SOURCES} ${HEADERS} Icon.ico ${RESOURCE_FILES} ${DATA_FILES})
endif()

file(GLOB FONT_SOURCES "src/Fonts/*.cpp")
set_source_files_properties(${FONT_SOURCES} PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)

group_files_by_directory("SOURCES")
group_files_by_directory("HEADERS")
source_group("Data" FILES ${DATA_FILES})

target_link_libraries(Editor PRIVATE Engine OpenFBX)

target_include_directories(Editor PUBLIC
                            ${CMAKE_CURRENT_SOURCE_DIR}/src
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/IconFont
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/OpenFBX/src
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/json
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/tinygltf
                            Engine/src
                            Engine/vendor
                            Engine/vendor/spdlog/include
                            Engine/vendor/cereal/include
                            Engine/vendor/box2d/include
                            Engine/vendor/lua
                            Engine/vendor/stb
        )

target_precompile_headers(Editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/stdafx.hpp)

add_definitions(
    -D_CRT_SECURE_NO_WARNINGS
    -DUNICODE
)

if(APPLE)
        add_custom_command(
        TARGET Editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/imgui.ini"
            "$<TARGET_FILE_DIR:Editor>/../Resources"
        COMMENT "Copying imgui.ini into app bundle Resources"
    )
else()
    add_custom_command(
        TARGET Editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Editor>/data"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/data"
            "$<TARGET_FILE_DIR:Editor>/data"
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/imgui.ini
            "$<TARGET_FILE_DIR:Editor>"
        COMMENT "Copying data folder and imgui.ini to binary dir"
    )
endif()

set_target_properties(Editor PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

if (EMSCRIPTEN)
    set_target_properties(Editor PROPERTIES SUFFIX ".html")
    target_link_options(Editor PRIVATE "--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/data@/data")
endif()

add_dependencies(Editor Runtime)
target_copy_webgpu_binaries(Editor)

if(APPLE)
    set_target_properties(Editor PROPERTIES INSTALL_RPATH "@executable_path")

    # A POST_BUILD command on the Editor target only reruns when Editor's own sources change,
    # not when Runtime's do - a Runtime-only rebuild used to leave a stale Runtime.app sitting
    # inside Editor.app. A stamp file tracked as this custom command's OUTPUT, depending on
    # Runtime's actual binary as an input, reruns the copy whenever Runtime.app itself is newer,
    # regardless of what triggered the build. Editor.app's own location is written as a plain
    # path (matching MACOSX_BUNDLE_BUNDLE_NAME/RUNTIME_OUTPUT_DIRECTORY above) rather than via
    # $<TARGET_BUNDLE_CONTENT_DIR:Editor> - referencing Editor's target here at all, genex or
    # not, makes CMake add an implicit dependency on it, which cycles against Editor depending
    # on this copy target below.
    set(RUNTIME_COPY_STAMP "${CMAKE_CURRENT_BINARY_DIR}/RuntimeCopied.stamp")
    add_custom_command(
        OUTPUT "${RUNTIME_COPY_STAMP}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_BINARY_DIR}/Runtime/Runtime.app"
            "${CMAKE_CURRENT_BINARY_DIR}/Editor.app/Contents/Resources/Runtime.app"
        COMMAND ${CMAKE_COMMAND} -E touch "${RUNTIME_COPY_STAMP}"
        DEPENDS Runtime "$<TARGET_FILE:Runtime>"
        COMMENT "Copying Runtime.app into Editor.app bundle"
    )
    # copy_directory creates its destination as needed, so this doesn't need Editor.app to exist
    # first - Editor depending on this target (rather than the other way around) just guarantees
    # it's present by the time a build of Editor finishes, however Editor is invoked (`--target
    # Editor` alone included, not just a default/"all" build).
    add_custom_target(CopyRuntimeIntoEditorBundle DEPENDS "${RUNTIME_COPY_STAMP}")
    add_dependencies(Editor CopyRuntimeIntoEditorBundle)
endif()

if (MSVC)
    target_compile_options(Editor PRIVATE "/utf-8")
endif()

# Runs Editor headless with --generate-docs, dumping the Lua API registry (populated by
# LuaManager::Init(), which app->Init() always calls before any window/renderer setup) to
# Markdown. --generate-docs takes the same "<some existing file> --generate-docs <dir>"
# form as --export-game purely to satisfy Application::Init's argument validation, which
# requires a real, openable argv[1] - the file's content is never read for this flag, so
# this project's own CMakeLists.txt stands in for it. Not part of the default build; run
# explicitly with `cmake --build . --target GenerateLuaDocs`.
#
# Also copies the engine logo into docs/assets/ for mkdocs.yml's theme.logo/favicon - it's a
# copy of Editor/data/Icons/Logo.png (the one real file, also used for the window icon and
# README), not a second asset to keep in sync by hand, so it's produced here alongside the
# generated Markdown rather than committed.
add_custom_target(GenerateLuaDocs
    COMMAND $<TARGET_FILE:Editor> "${CMAKE_SOURCE_DIR}/CMakeLists.txt" --generate-docs "${CMAKE_SOURCE_DIR}/docs/LuaAPI"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/docs/assets"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/Editor/data/Icons/Logo.png" "${CMAKE_SOURCE_DIR}/docs/assets/logo.png"
    DEPENDS Editor
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Generating Lua API documentation"
)