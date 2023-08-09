add_library(miniz
    OpenFBX/src/miniz.c
    OpenFBX/src/miniz.h
)

set_target_properties(miniz PROPERTIES
    C_STANDARD 90
    C_STANDARD_REQUIRED ON
)

if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3 /Zi /permissive-")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wshadow -Wformat=2 -Wall -Wno-overlength-strings -pedantic")
endif()

add_library(OpenFBX 
    OpenFBX/src/ofbx.cpp
    OpenFBX/src/ofbx.h
)

set_target_properties(OpenFBX PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_compile_definitions(OpenFBX PRIVATE _LARGEFILE64_SOURCE)

target_link_libraries(OpenFBX PUBLIC
                    miniz
)