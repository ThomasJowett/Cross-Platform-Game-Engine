#pragma once

#include "Scene/SceneCamera.h"

#include "cereal/cereal.hpp"
#include "Scripting/Lua/LuaBindings.h"

struct CameraComponent
{
	SceneCamera camera;
	bool primary = true;
	bool fixedAspectRatio = false;

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;

	operator SceneCamera& () { return camera; }
	operator const SceneCamera& () const { return camera; }
	REFLECT_LUA_BEGIN(CameraComponent)
		REFLECT_LUA_PROPERTY_CUSTOM("Camera", "The camera's projection settings. Returns a copy - assign it back after changing it", "Camera",
			([](Self& c) { return c.camera; }),
			([](Self& c, const SceneCamera& v) { c.camera = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Primary", "Whether this is the camera the scene is rendered from at runtime", "boolean",
			([](Self& c) { return c.primary; }),
			([](Self& c, bool v) { c.primary = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("FixedAspectRatio", "Keep the camera's own aspect ratio instead of matching the viewport", "boolean",
			([](Self& c) { return c.fixedAspectRatio; }),
			([](Self& c, bool v) { c.fixedAspectRatio = v; }))
	REFLECT_LUA_END()

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(camera, primary, fixedAspectRatio);
	}
};