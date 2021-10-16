#pragma once

#include <filesystem>

#include "cereal/cereal.hpp"
#include "cereal/access.hpp"

#include "Core/Colour.h"
#include "Renderer/Texture.h"

#include "Utilities/FileUtils.h"
#include "Core/Application.h"

class Tileset
{
	struct Tile
	{
		int Id;
		std::string Type;
		double Probability = 1.0f;

	};
public:
	Tileset() = default;

	bool Load(std::filesystem::path& filepath);
	bool Save(const std::filesystem::path& filepath) const;

private:
	std::string m_Name;
	Ref<Texture2D> m_Texture;

	uint32_t m_TileWidth;
	uint32_t m_TileHeight;

	uint32_t m_TileCount;
	uint32_t m_Columns;

	Colour m_BackgroundColour;

	uint32_t m_OffsetX, m_OffsetY;

	std::filesystem::path m_Filepath;

	std::vector<Tile> m_Tiles;
};

/* ------------------------------------------------------------------------------------------------------------------ */

class Tilemap
{
	enum class Orientation
	{
		orthogonal,
		isometric,
		staggered,
		hexagonal
	};

	enum class RenderingOrder
	{
		rightDown,
		rightUp,
		leftDown,
		leftUp
	};

	enum class StaggerAxis
	{
		X,
		Y
	};

	enum class StaggerIndex
	{
		even,
		odd
	};

	class Layer
	{
		uint32_t m_Id;
		std::string m_Name;
		uint32_t m_Width, m_Height;

		Vector2f m_Offset;
		Vector2f m_ParralaxFactor;

		uint32_t** m_Tiles;

	public:
		Layer(uint32_t id, const std::string& name, uint32_t width, uint32_t height, Vector2f offset);

		bool ParseCsv(const std::string& data);

		void Resize(uint32_t width, uint32_t height);
		void Offset(float vertical, float horizontal);
	};

public:
	Tilemap() = default;
	bool Load(std::filesystem::path);
	bool Save(const std::filesystem::path& filepath) const;

private:
	std::string m_Name;
	std::filesystem::path m_Filepath;

	Orientation m_Orientation;
	RenderingOrder m_RenderingOrder;

	bool m_IsStaggered;

	uint32_t m_Width, m_Height;
	uint32_t m_TileWidth, m_TileHeight;

	uint32_t m_HexSideLength;
	StaggerAxis m_StaggerAxis;
	StaggerIndex m_StaggerIndex;

	Colour m_BackgroundColour;

	bool m_Infinite;

	std::vector<std::pair<Tileset, uint32_t>> m_Tilesets;

	std::vector<Layer> m_Layers;

	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		Save(m_Filepath);
		std::string relativePath = FileUtils::relativePath(m_Filepath, Application::GetOpenDocumentDirectory());
		archive(cereal::make_nvp("Filepath", relativePath));
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		std::string relativePath;
		archive(cereal::make_nvp("Filepath", relativePath));

		m_Filepath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath);
		Load(m_Filepath);
	}
};