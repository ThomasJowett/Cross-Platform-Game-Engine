#pragma once

#include "math/Vector2f.h"
#include "math/Vector3f.h"

#include "Renderer/Material.h"

#include "Core/Colour.h"

#include "TinyXml2/tinyxml2.h"


namespace SerializationUtils
{
	void Encode(tinyxml2::XMLElement* pElement, const Vector2f& vec2);

	void Decode(tinyxml2::XMLElement* pElement, Vector2f& vec2);

	void Encode(tinyxml2::XMLElement* pElement, const Vector3f& vec3);

	void Decode(tinyxml2::XMLElement* pElement, Vector3f& vec3);

	void Encode(tinyxml2::XMLElement* pElement, const Colour& colour);

	void Decode(tinyxml2::XMLElement* pElement, Colour& colour);

	void Encode(tinyxml2::XMLElement* pElement, const Material& material);

	void Decode(tinyxml2::XMLElement* pElement, Material& material);

	std::string RelativePath(const std::filesystem::path& path);
	std::filesystem::path AbsolutePath(const char* path);
}