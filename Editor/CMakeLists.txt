file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FileSystem/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Fonts/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/History/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ImGui/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Importers/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Interfaces/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Panels/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Styles/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Toolbars/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Viewers/*.cpp
)

file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FileSystem/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Fonts/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/History/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ImGui/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Importers/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Interfaces/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Panels/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Styles/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Toolbars/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Viewers/*.h
)

add_executable(Editor ${SOURCES} ${HEADERS})

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

target_link_libraries(Editor PRIVATE Engine)

target_include_directories(Editor PUBLIC
                            ${CMAKE_CURRENT_SOURCE_DIR}/src
                            ${CMAKE_CURRENT_SOURCE_DIR}/vendor
                            Engine/src
                            Engine/vendor
        )