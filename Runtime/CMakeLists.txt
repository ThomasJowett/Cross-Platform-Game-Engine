add_executable(Runtime src/main.cpp
                src/RuntimeLayer.cpp
                src/RuntimeLayer.h)

target_link_libraries(Runtime PRIVATE Engine miniz)

target_include_directories(Runtime PUBLIC
                            ${CMAKE_SOURCE_DIR}/Editor/vendor/OpenFBX/src
)

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