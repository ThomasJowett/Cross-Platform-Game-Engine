#ifndef _PCH_
#define _PCH_

// Standard C++
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <array>
#include <set>
#include <string>
#include <sstream>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <math.h>
#include <float.h>
#include <any>
#include <filesystem>
#include <numeric>

#ifdef _WINDOWS
#ifdef __MINGW32__
	#define WIN32_LEAN_AND_MEAN
#else
	#define NOMINMAX
#endif
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#endif // _WINDOWS

// math libraries
#include "math/Line3D.h"
#include "math/Matrix.h"
#include "math/Plane.h"
#include "math/Quaternion.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"

// EnTT
#include "EnTT/entt.hpp"

#endif // !_PCH_