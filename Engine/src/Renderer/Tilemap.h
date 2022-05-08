#pragma once

#include <filesystem>

#include "cereal/cereal.hpp"
#include "cereal/access.hpp"

#include "Core/Colour.h"
#include "Renderer/Texture.h"
#include "Renderer/SubTexture2D.h"

#include "Utilities/FileUtils.h"
#include "Core/Application.h"

class Tileset
{
	struct Tile
	{
		int id = -1;
		std::string type;
		double probability = 1.0f;

	};
public:
	Tileset() = default;

	bool Load(std::filesystem::path& filepath);
	bool Save(const std::filesystem::path& filepath) const;

	const std::filesystem::path& Source() const { return m_Filepath; }
	const Ref<SubTexture2D> GetTexture() const { return m_Texture; }

	const uint32_t GetTileWidth() const { return m_TileWidth; }
	const uint32_t GetTileHeight() const { return m_TileHeight; }

	void SetCurrentTile(uint32_t tile) { m_Texture->SetCurrentCell(tile); }
private:
	std::string m_Name;
	Ref<SubTexture2D> m_Texture;

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
public:
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
		void SetOffset(float vertical, float horizontal) { m_Offset = { horizontal, vertical }; }
		Vector2f GetOffset() const { return m_Offset; }
		
		uint32_t GetTile(uint32_t x, uint32_t y) const { return m_Tiles[y][x]; }
		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
	};

	struct Object
	{
		uint32_t id;
		std::string name;
		std::string type;
		bool visible = true;
		Vector2f position;
		float rotation;
	};

	struct RectangleObject : public Object
	{
		float m_Width;
		float m_Height;
	};

	struct EllipseObject : public Object
	{

	};

	struct PolygonObject : public Object
	{
		std::vector<Vector2f> m_Points;
	};

	struct TextObject : public Object
	{
		std::string m_Text;
		bool m_WordWrap;
		Colour m_Colour;
	};

	struct Tile : public Object
	{
		uint32_t m_Gid;
	};

public:
	Tilemap() = default;
	bool Load(std::filesystem::path);
	bool Save(const std::filesystem::path& filepath) const;
	bool Save() const;

	void Render(const Matrix4x4& transform, int entityId);

	const std::filesystem::path& GetFilepath() const { return m_Filepath; }
	Orientation GetOrientation() const { return m_Orientation; }
	void SetOrientation(Orientation orientaion) { m_Orientation = orientaion; }

	uint32_t GetWidth() const { return m_Width; }
	void SetWidth(uint32_t width) { m_Width = width; }

	uint32_t GetHeight() const { return m_Height; }
	void SetHeight(uint32_t height) { m_Height = height; }

	uint32_t GetTileWidth() const { return m_TileWidth; }
	void SetTileWidth(uint32_t tileWidth) { m_TileWidth = tileWidth; }

	uint32_t GetTileHeight() const { return m_TileHeight; }
	void SetTileHeight(uint32_t tileHeight) { m_TileHeight = tileHeight; }

private:
	std::string m_Name;
	std::filesystem::path m_Filepath;

	Orientation m_Orientation = Orientation::orthogonal;
	RenderingOrder m_RenderingOrder = RenderingOrder::rightDown;

	bool m_IsStaggered = false;

	uint32_t m_Width, m_Height;
	uint32_t m_TileWidth, m_TileHeight;

	uint32_t m_HexSideLength;
	StaggerAxis m_StaggerAxis;
	StaggerIndex m_StaggerIndex;

	Colour m_BackgroundColour;

	bool m_Infinite;

	std::map<uint32_t, Ref<Tileset>> m_Tilesets;

	std::vector<Layer> m_Layers;
	std::vector<Object> m_Objects;

	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		if (!m_Filepath.empty())
		{
			Save(m_Filepath);
		}
		std::string relativePath = FileUtils::RelativePath(m_Filepath, Application::GetOpenDocumentDirectory()).string();
		archive(cereal::make_nvp("Filepath", relativePath));
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		std::string relativePath;
		archive(cereal::make_nvp("Filepath", relativePath));

		if (!relativePath.empty())
		{
			m_Filepath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath);
			Load(m_Filepath);
		}
	}
};