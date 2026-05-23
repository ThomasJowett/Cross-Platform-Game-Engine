file(GLOB SOURCES
    src/*.cpp
    src/*.h
)

if(APPLE)
    add_executable(Runtime MACOSX_BUNDLE ${SOURCES})
    set_target_properties(Runtime PROPERTIES
        BUNDLE True
        MACOSX_BUNDLE_BUNDLE_NAME "Runtime"
        MACOSX_BUNDLE_EXECUTABLE_NAME "Runtime"
    )
else()
    add_executable(Runtime ${SOURCES})
endif()

target_link_libraries(Runtime PRIVATE Engine)

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

if (MSVC)
    target_compile_options(Runtime PRIVATE "/utf-8")
endif()