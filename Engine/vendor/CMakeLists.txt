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
    GLAD/include/glad/glad.h
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

add_library(freetype STATIC
    freetype/include/ft2build.h
    freetype/src/autofit/autofit.c
	freetype/src/base/ftbase.c
	freetype/src/base/ftbbox.c
	freetype/src/base/ftbdf.c
	freetype/src/base/ftbitmap.c
	freetype/src/base/ftcid.c
	freetype/src/base/ftdebug.c
	freetype/src/base/ftfstype.c
	freetype/src/base/ftgasp.c
	freetype/src/base/ftglyph.c
	freetype/src/base/ftgxval.c
	freetype/src/base/ftinit.c
	freetype/src/base/ftmm.c
	freetype/src/base/ftotval.c
	freetype/src/base/ftpatent.c
	freetype/src/base/ftpfr.c
	freetype/src/base/ftstroke.c
	freetype/src/base/ftsynth.c
	freetype/src/base/ftsystem.c
	freetype/src/base/fttype1.c
	freetype/src/base/ftwinfnt.c
	freetype/src/bdf/bdf.c
	freetype/src/bzip2/ftbzip2.c
	freetype/src/cache/ftcache.c
	freetype/src/cff/cff.c
	freetype/src/cid/type1cid.c
	freetype/src/gzip/ftgzip.c
	freetype/src/lzw/ftlzw.c
	freetype/src/pcf/pcf.c
	freetype/src/pfr/pfr.c
	freetype/src/psaux/psaux.c
	freetype/src/pshinter/pshinter.c
	freetype/src/psnames/psnames.c
	freetype/src/raster/raster.c
	freetype/src/sdf/sdf.c
	freetype/src/sfnt/sfnt.c
	freetype/src/smooth/smooth.c
	freetype/src/truetype/truetype.c
	freetype/src/type1/type1.c
	freetype/src/type42/type42.c
	freetype/src/winfonts/winfnt.c
)

target_include_directories(freetype PUBLIC
    freetype/include
)

set_target_properties(freetype PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

target_compile_definitions(freetype PRIVATE
    FT2_BUILD_LIBRARY
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_WARNINGS
)

add_library(lua STATIC
    lua/lapi.c
    lua/lapi.h
    lua/lauxlib.c
    lua/lauxlib.h
    lua/lbaselib.c
    lua/lcode.c
    lua/lcode.h
    lua/lcorolib.c
    lua/lctype.c
    lua/lctype.h
    lua/ldblib.c
    lua/ldebug.c
    lua/ldebug.h
    lua/ldo.c
    lua/ldo.h
    lua/ldump.c
    lua/lfunc.c
    lua/lfunc.h
    lua/lgc.c
    lua/lgc.h
    lua/linit.c
    lua/liolib.c
    lua/ljumptab.h
    lua/llex.c
    lua/llex.h
    lua/llimits.h
    lua/lmathlib.c
    lua/lmem.c
    lua/lmem.h
    lua/loadlib.c
    lua/lobject.c
    lua/lobject.h
    lua/lopcodes.c
    lua/lopcodes.h
    lua/lopnames.h
    lua/loslib.c
    lua/lparser.c
    lua/lparser.h
    lua/lprefix.h
    lua/lstate.c
    lua/lstate.h
    lua/lstring.c
    lua/lstring.h
    lua/lstrlib.c
    lua/ltable.c
    lua/ltable.h
    lua/ltablib.c
    lua/ltests.c
    lua/ltests.h
    lua/ltm.c
    lua/ltm.h
    lua/lua.h
    lua/luaconf.h
    lua/lualib.h
    lua/lundump.c
    lua/lundump.h
    lua/lutf8lib.c
    lua/lvm.c
    lua/lvm.h
    lua/lzio.c
    lua/lzio.h
)

set_target_properties(lua PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

target_compile_definitions(freetype PRIVATE
    LUA_COMPAT_MATHLIB=1
)

file(GLOB MSDFGEN_FILES
    msdf-atlas-gen/msdfgen/core/*.h
    msdf-atlas-gen/msdfgen/core/*.hpp
    msdf-atlas-gen/msdfgen/core/*.cpp
    msdf-atlas-gen/msdfgen/ext/*.h
    msdf-atlas-gen/msdfgen/ext/*.hpp
    msdf-atlas-gen/msdfgen/ext/*.cpp
    msdf-atlas-gen/msdfgen/lib/*.cpp
    msdf-atlas-gen/msdfgen/include/*.h
)

add_library(msdfgen STATIC
    ${MSDFGEN_FILES}
)

set_target_properties(msdfgen PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(msdfgen PUBLIC
    msdf-atlas-gen/msdfgen/include
    freetype/include
)

target_compile_definitions(msdfgen PRIVATE
    MSDFGEN_USE_CPP11
)

target_link_libraries(msdfgen PUBLIC
    freetype
)

file(GLOB MSDF_ATLAS_GEN_FILES
    msdf-atlas-gen/msdf-atlas-gen/**.h
    msdf-atlas-gen/msdf-atlas-gen/**.hpp
    msdf-atlas-gen/msdf-atlas-gen/**.cpp
)

add_library(msdf-atlas-gen STATIC
    ${MSDF_ATLAS_GEN_FILES}
)

set_target_properties(msdf-atlas-gen PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(msdf-atlas-gen PUBLIC
    msdf-atlas-gen/msdf-atlas-gen
    msdf-atlas-gen/msdfgen
    msdf-atlas-gen/msdfgen/include
    msdf-atlas-gen/artery-font-format
)

target_compile_definitions(msdf-atlas-gen PRIVATE
    _CRT_SECURE_NO_WARNINGS
)

target_link_libraries(msdf-atlas-gen PUBLIC
    msdfgen
)