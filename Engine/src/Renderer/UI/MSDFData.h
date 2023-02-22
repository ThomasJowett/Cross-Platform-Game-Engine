#pragma once

#undef INFINITE
#include "msdf-atlas-gen.h"

#include <vector>

struct MSDFData
{
	msdf_atlas::FontGeometry fontGeometry;
	std::vector<msdf_atlas::GlyphGeometry> glyphs;
};
