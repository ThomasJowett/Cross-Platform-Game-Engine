#include "stdafx.h"
#include "LuaBindings.h"

#include "Logging/Instrumentor.h"
#include "LuaManager.h"

namespace Lua
{
void BindMath(sol::state& state)
{
	PROFILE_FUNCTION();

	sol::usertype<Vector2f> vector2_type = state.new_usertype<Vector2f>(
		"Vec2",
		sol::constructors<Vector2f(float, float), Vector2f()>(),
		"x", &Vector2f::x,
		"y", &Vector2f::y,
		sol::meta_function::addition, [](const Vector2f& a, const Vector2f& b) { return a + b; },
		sol::meta_function::subtraction, [](const Vector2f& a, const Vector2f& b) { return a - b; },
		sol::meta_function::multiplication, [](const Vector2f& a, const float& b) {return a * b; },
		sol::meta_function::unary_minus, [](Vector2f const& a) {return -a; }
	);

	vector2_type.set_function("Length", &Vector2f::Magnitude);
	vector2_type.set_function("SqrLength", &Vector2f::SqrMagnitude);
	vector2_type.set_function("Normalize", &Vector2f::Normalize);
	vector2_type.set_function("Clamp", &Vector2f::Clamp);
	vector2_type.set_function("Perpendicular", &Vector2f::Perpendicular);
	vector2_type.set_function("Dot", &Vector2f::Dot);
	vector2_type.set_function("Distance", &Vector2f::Distance);
	vector2_type.set_function("SqrDistance", &Vector2f::SqrDistance);
	vector2_type.set_function("Lerp", &Vector2f::Lerp);
	vector2_type.set_function("Angle", &Vector2f::Angle);
	vector2_type.set_function("Cross", &Vector2f::Cross);
	vector2_type.set_function("Perpendicular", &Vector2f::Perpendicular);
	vector2_type.set_function("Reflect", &Vector2f::Reflect);
	vector2_type.set_function("Zero", &Vector2f::Zero);

	sol::usertype<Vector3f> vector3_type = state.new_usertype<Vector3f>(
		"Vec3",
		sol::constructors<Vector3f(float, float, float), Vector3f()>(),
		"x", &Vector3f::x,
		"y", &Vector3f::y,
		"z", &Vector3f::z,
		sol::meta_function::addition, [](const Vector3f& a, const Vector3f& b) { return a + b; },
		sol::meta_function::subtraction, [](const Vector3f& a, const Vector3f& b) { return a - b; },
		sol::meta_function::multiplication, [](const Vector3f& a, const float& b) {return a * b; },
		sol::meta_function::unary_minus, [](Vector3f const& a) {return -a; }
	);

	vector3_type.set_function("Length", &Vector3f::Magnitude);
	vector3_type.set_function("SqrLength", &Vector3f::SqrMagnitude);
	vector3_type.set_function("Normalize", &Vector3f::Normalize);
	vector3_type.set_function("Clamp", &Vector3f::Clamp);
	vector3_type.set_function("Dot", &Vector3f::Dot);
	vector3_type.set_function("Distance", &Vector3f::Distance);
	vector3_type.set_function("Lerp", &Vector3f::Lerp);
	vector3_type.set_function("Cross", &Vector3f::Cross);
	vector3_type.set_function("Reflect", &Vector3f::Reflect);
	vector3_type.set_function("Zero", &Vector3f::Zero);

	sol::usertype<Quaternion> quaternion_type = state.new_usertype<Quaternion>(
		"Quaternion",
		sol::constructors<Quaternion(float, float, float), Quaternion()>(),
		"w", &Quaternion::w,
		"x", &Quaternion::x,
		"y", &Quaternion::y,
		"z", &Quaternion::z,
		sol::meta_function::addition, [](const Quaternion& a, const Quaternion& b) { return a + b; },
		sol::meta_function::addition, [](const Quaternion& a, const Quaternion& b) { return a - b; }
	);

	quaternion_type.set_function("EulerAngles", &Quaternion::EulerAngles);
	quaternion_type.set_function("Length", &Quaternion::GetMagnitude);
	quaternion_type.set_function("SqrLength", &Quaternion::GetSqrMagnitude);
	quaternion_type.set_function("Normalize", &Quaternion::Normalize);
	quaternion_type.set_function("GetNormalized", &Quaternion::GetNormalized);
	quaternion_type.set_function("Conjugate", &Quaternion::Conjugate);
	quaternion_type.set_function("Inverse", &Quaternion::Inverse);
}
}