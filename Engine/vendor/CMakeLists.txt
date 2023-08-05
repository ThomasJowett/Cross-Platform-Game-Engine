add_library(imgui STATIC
    imgui/imconfig.h
    imgui/imgui.h
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_internal.h
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    imgui/imgui_demo.cpp
)

target_compile_definitions(imgui PRIVATE
    _CRT_SECURE_NO_WARNINGS
)

set_target_properties(imgui PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

add_library(glad STATIC
    GLAD/include/GLAD/glad.h
    GLAD/include/KHR/khrplatform.h
    GLAD/src/glad.c
)

target_include_directories(glad PUBLIC
    GLAD/include
)

set_target_properties(glad PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)