add_library(OpenFBX 
    OpenFBX/src/miniz.c
    OpenFBX/src/miniz.h
    OpenFBX/src/ofbx.cpp
    OpenFBX/src/ofbx.h
)

set_target_properties(OpenFBX PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_compile_definitions(OpenFBX PRIVATE _LARGEFILE64_SOURCE)