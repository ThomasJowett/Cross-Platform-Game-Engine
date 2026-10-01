#include "LuaBindings.h"

#include "Logging/Instrumentor.h"
#include "LuaManager.h"
#include "math/Quaternion.h"
#include "math/Vector2f.h"
#include "sol/property.hpp"

namespace Lua
{
void BindMath(sol::state& state)
{
	PROFILE_FUNCTION();

	sol::usertype<Vector2f> vector2_type = state.new_usertype<Vector2f>(
		"Vec2",
		sol::constructors<Vector2f(float, float), Vector2f()>(),
		"x", sol::property([](Vector2f& v) { return v.x; }, [](Vector2f& v, float value) { v.x = value; }),
		"y", sol::property([](Vector2f& v) { return v.y; }, [](Vector2f& v, float value) { v.y = value; }),
		sol::meta_function::addition, [](const Vector2f& a, const Vector2f& b) { return a + b; },
		sol::meta_function::subtraction, [](const Vector2f& a, const Vector2f& b) { return a - b; },
		sol::meta_function::multiplication, [](const Vector2f& a, const float& b) {return a * b; },
		sol::meta_function::unary_minus, [](Vector2f const& a) {return -a; }
	);

	SetFunction(vector2_type, "Vec2", "Length", "Get the vector's length", &Vector2f::Magnitude);
	SetFunction(vector2_type, "Vec2", "SqrLength", "Get the squared length, cheaper than Length for comparisons", &Vector2f::SqrMagnitude);
	SetFunction(vector2_type, "Vec2", "Normalize", "Scale this vector to length 1, in place", &Vector2f::Normalize);
	SetFunction(vector2_type, "Vec2", "Clamp", "Shorten this vector in place to at most the given length", &Vector2f::Clamp);
	SetFunction(vector2_type, "Vec2", "Perpendicular", "Get the vector rotated 90 degrees: Perpendicular(clockwise)", &Vector2f::Perpendicular);
	SetFunction(vector2_type, "Vec2", "Dot", "Vec2.Dot(a, b): dot product", &Vector2f::Dot);
	SetFunction(vector2_type, "Vec2", "Distance", "Vec2.Distance(a, b): distance between two points", &Vector2f::Distance);
	SetFunction(vector2_type, "Vec2", "SqrDistance", "Vec2.SqrDistance(a, b): squared distance between two points", &Vector2f::SqrDistance);
	SetFunction(vector2_type, "Vec2", "Lerp", "Vec2.Lerp(a, b, alpha): blend from a to b", &Vector2f::Lerp);
	SetFunction(vector2_type, "Vec2", "Angle", "Vec2.Angle(a, b): angle between two vectors, in radians", &Vector2f::Angle);
	SetFunction(vector2_type, "Vec2", "Cross", "Vec2.Cross(a, b): 2D cross product", &Vector2f::Cross);
	SetFunction(vector2_type, "Vec2", "Reflect", "Vec2.Reflect(v, normal): bounce a vector off a surface", &Vector2f::Reflect);
	SetFunction(vector2_type, "Vec2", "Zero", "Set this vector to 0, 0 in place", &Vector2f::Zero);

	sol::usertype<Vector3f> vector3_type = state.new_usertype<Vector3f>(
		"Vec3",
		sol::constructors<Vector3f(float, float, float), Vector3f()>(),
		"x", sol::property([](Vector3f& v) { return v.x; }, [](Vector3f& v, float value) { v.x = value; }),
		"y", sol::property([](Vector3f& v) { return v.y; }, [](Vector3f& v, float value) { v.y = value; }),
		"z", sol::property([](Vector3f& v) { return v.z; }, [](Vector3f& v, float value) { v.z = value; }),
		sol::meta_function::addition, [](const Vector3f& a, const Vector3f& b) { return a + b; },
		sol::meta_function::subtraction, [](const Vector3f& a, const Vector3f& b) { return a - b; },
		sol::meta_function::multiplication, [](const Vector3f& a, const float& b) {return a * b; },
		sol::meta_function::unary_minus, [](Vector3f const& a) {return -a; }
	);

	SetFunction(vector3_type, "Vec3", "Length", "Get the vector's length", &Vector3f::Magnitude);
	SetFunction(vector3_type, "Vec3", "SqrLength", "Get the squared length, cheaper than Length for comparisons", &Vector3f::SqrMagnitude);
	SetFunction(vector3_type, "Vec3", "Normalize", "Scale this vector to length 1, in place", &Vector3f::Normalize);
	SetFunction(vector3_type, "Vec3", "Clamp", "Shorten this vector in place to at most the given length", &Vector3f::Clamp);
	SetFunction(vector3_type, "Vec3", "Dot", "Vec3.Dot(a, b): dot product", &Vector3f::Dot);
	SetFunction(vector3_type, "Vec3", "Distance", "Vec3.Distance(a, b): distance between two points", &Vector3f::Distance);
	SetFunction(vector3_type, "Vec3", "Lerp", "Vec3.Lerp(a, b, alpha): blend from a to b", &Vector3f::Lerp);
	SetFunction(vector3_type, "Vec3", "Cross", "Vec3.Cross(a, b): cross product", &Vector3f::Cross);
	SetFunction(vector3_type, "Vec3", "Reflect", "Vec3.Reflect(v, normal): bounce a vector off a surface", &Vector3f::Reflect);
	SetFunction(vector3_type, "Vec3", "Zero", "Set this vector to 0, 0, 0 in place", &Vector3f::Zero);

	sol::usertype<Quaternion> quaternion_type = state.new_usertype<Quaternion>(
		"Quaternion",
		sol::constructors<Quaternion(float, float, float), Quaternion()>(),
		"w", sol::property([](Quaternion& q) { return q.w; }, [](Quaternion& q, float value) { q.w = value; }),
		"x", sol::property([](Quaternion& q) { return q.x; }, [](Quaternion& q, float value) { q.x = value; }),
		"y", sol::property([](Quaternion& q) { return q.y; }, [](Quaternion& q, float value) { q.y = value; }),
		"z", sol::property([](Quaternion& q) { return q.z; }, [](Quaternion& q, float value) { q.z = value; }),
		sol::meta_function::addition, [](const Quaternion& a, const Quaternion& b) { return a + b; },
		sol::meta_function::subtraction, [](const Quaternion& a, const Quaternion& b) { return a - b; }
	);

	SetFunction(quaternion_type, "Quaternion", "EulerAngles", "Get the rotation as Euler angles in radians, as a Vec3", &Quaternion::EulerAngles);
	SetFunction(quaternion_type, "Quaternion", "Length", "Get the quaternion's length", &Quaternion::GetMagnitude);
	SetFunction(quaternion_type, "Quaternion", "SqrLength", "Get the squared length", &Quaternion::GetSqrMagnitude);
	SetFunction(quaternion_type, "Quaternion", "Normalize", "Scale to length 1, in place", &Quaternion::Normalize);
	SetFunction(quaternion_type, "Quaternion", "GetNormalized", "Get a copy scaled to length 1", &Quaternion::GetNormalized);
	SetFunction(quaternion_type, "Quaternion", "Conjugate", "Get the conjugate", &Quaternion::Conjugate);
	SetFunction(quaternion_type, "Quaternion", "Inverse", "Get the inverse rotation", &Quaternion::Inverse);
	// Constructors, operators and fields are set up in new_usertype above; listed here for the docs
	RegisterLuaApiEntry({ "new", "Vec2.new(x, y), or Vec2.new() for 0, 0. Supports +, - and * by a number", LuaApiEntry::Kind::Function, "Vec2", "" });
	RegisterLuaApiEntry({ "x", "X component", LuaApiEntry::Kind::Property, "Vec2", "number" });
	RegisterLuaApiEntry({ "y", "Y component", LuaApiEntry::Kind::Property, "Vec2", "number" });
	RegisterLuaApiEntry({ "new", "Vec3.new(x, y, z), or Vec3.new() for 0, 0, 0. Supports +, - and * by a number", LuaApiEntry::Kind::Function, "Vec3", "" });
	RegisterLuaApiEntry({ "x", "X component", LuaApiEntry::Kind::Property, "Vec3", "number" });
	RegisterLuaApiEntry({ "y", "Y component", LuaApiEntry::Kind::Property, "Vec3", "number" });
	RegisterLuaApiEntry({ "z", "Z component", LuaApiEntry::Kind::Property, "Vec3", "number" });
	RegisterLuaApiEntry({ "new", "Quaternion.new(roll, pitch, yaw) from Euler angles in radians, or Quaternion.new() for no rotation. Supports + and -", LuaApiEntry::Kind::Function, "Quaternion", "" });
	RegisterLuaApiEntry({ "w", "W component", LuaApiEntry::Kind::Property, "Quaternion", "number" });
	RegisterLuaApiEntry({ "x", "X component", LuaApiEntry::Kind::Property, "Quaternion", "number" });
	RegisterLuaApiEntry({ "y", "Y component", LuaApiEntry::Kind::Property, "Quaternion", "number" });
	RegisterLuaApiEntry({ "z", "Z component", LuaApiEntry::Kind::Property, "Quaternion", "number" });
}
}
