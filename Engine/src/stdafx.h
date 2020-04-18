#pragma once

// Standard C++
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <math.h>
#include <float.h>
#include <any>

#ifdef __WINDOWS__

#include <Windows.h>
#include <d3d11.h>

#endif // __WINDOWS__

// math libraries
//#include "math/Line3D.h"
#include "math/Matrix.h"
#include "math/Plane.h"
#include "math/Quaternion.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"

// Debug
#include "Logging/Instrumentor.h"
#include "Logging/Logger.h"