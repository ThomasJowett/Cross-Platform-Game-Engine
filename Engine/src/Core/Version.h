#pragma once

// The major version
#define VERSION_MAJOR 0
// The minor version
#define VERSION_MINOR 0
// The patch version
#define VERSION_PATCH 1

// The full version as a single number
#define VERSION (VERSION_MAJOR * 10000 \
				+ VERSION_MINOR * 100 \
				+ VERSION_PATCH)

#include "spdlog/version.h"
#include "cereal/version.hpp"