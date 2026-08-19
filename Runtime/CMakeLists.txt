file(GLOB SOURCES
    src/*.cpp
    src/*.h
)

file(GLOB_RECURSE DATA_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../Editor/data/*")

if(APPLE)
    foreach(file ${DATA_FILES})
        file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_SOURCE_DIR}/../Editor/data" ${file})
        get_filename_component(rel_dir "${rel_path}" DIRECTORY)
        set_source_files_properties(${file} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources/data/${rel_dir}")
    endforeach()
    add_executable(Runtime MACOSX_BUNDLE ${SOURCES} ${DATA_FILES})
    set_target_properties(Runtime PROPERTIES
        BUNDLE True
        MACOSX_BUNDLE_BUNDLE_NAME "Runtime"
        MACOSX_BUNDLE_EXECUTABLE_NAME "Runtime"
    )
else()
    add_executable(Runtime ${SOURCES} ${DATA_FILES})
endif()

source_group("Data" FILES ${DATA_FILES})

target_link_libraries(Runtime PRIVATE Engine)

if (EMSCRIPTEN)
    set_target_properties(Runtime PROPERTIES SUFFIX ".html")
    target_link_options(Runtime PRIVATE "--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/../Editor/data@/data")
endif()

if(NOT APPLE)
    get_target_property(EDITOR_BINARY_DIR Editor RUNTIME_OUTPUT_DIRECTORY)

    # Define the output directory base path
    set(OUTPUT_DIRECTORY_BASE "${EDITOR_BINARY_DIR}")
    # Loop over each configuration type and set the output directory to inside the editor directory
    foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${CONFIG} CONFIG_UPPER)
        set_target_properties(Runtime PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${OUTPUT_DIRECTORY_BASE}/${CONFIG}/runtime"
        )
    endforeach()
endif()

if(NOT APPLE)
    add_custom_command(
        TARGET Runtime POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Runtime>/data"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/../Editor/data"
            "$<TARGET_FILE_DIR:Runtime>/data"
        COMMENT "Copying data folder to Runtime binary dir"
    )
endif()

if (MSVC)
    target_compile_options(Runtime PRIVATE "/utf-8")
endif()