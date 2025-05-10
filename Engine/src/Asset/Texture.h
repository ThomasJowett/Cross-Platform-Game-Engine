#pragma once

#include <unordered_map>
#include <filesystem>

#include "Core/core.h"
#include "Core/Asset.h"

class Texture : public Asset
{
public:
	enum class Format
	{
		None = 0,
		RED,
		RED8UI,
		RED16UI,
		RED32UI,
		RED32F,
		RG8,
		RG16F,
		RG32F,
		RGB,
		RGBA,
		RGBA16F,
		RGBA32F,

		B10R11G11UF,

		SRGB,

		DEPTH32FSTENCIL8UINT,
		DEPTH32F,
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8,
	};

	enum class FilterMethod
	{
		Linear,
		Nearest
	};

	enum class WrapMethod
	{
		Clamp,
		Mirror,
		Repeat
	};

	virtual ~Texture() = default;
	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;

	virtual void SetData(const void* data) = 0;

	virtual void Bind(uint32_t slot = 0) const = 0;

	virtual uint32_t GetRendererID() const = 0;

	virtual void Reload() = 0;

	FilterMethod GetFilterMethod() const { return m_FilterMethod; }
	virtual void SetFilterMethod(FilterMethod filterMethod) { m_FilterMethod = filterMethod; }

	WrapMethod GetWrapMethod() const { return m_WrapMethod; }
	virtual void SetWrapMethod(WrapMethod wrapMethod) { m_WrapMethod = wrapMethod; }

	virtual bool operator==(const Texture& other) const = 0;

	FilterMethod m_FilterMethod = FilterMethod::Nearest;
	WrapMethod m_WrapMethod = WrapMethod::Repeat;
};

class Texture2D :public Texture
{
public:
	virtual bool Load(const std::filesystem::path& filepath) override;
	virtual bool Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data) override;
	static Ref<Texture2D> Create(uint32_t width, uint32_t height, Format format = Format::RGBA, const void* pixels = nullptr);
	static Ref<Texture2D> Create(const std::filesystem::path& filepath);
	static Ref<Texture2D> Create(const std::filesystem::path& filepath, const std::vector<uint8_t>& imageData);
};

class TextureLibrary2D
{
public:
	void Add(const Ref<Texture2D>& texture);
	Ref<Texture2D> Load(const std::filesystem::path& path, Ref<VirtualFileSystem> vfs);
	Ref<Texture2D> Get(const std::filesystem::path& name);
	void Clear();
	size_t Size() const { return m_Textures.size(); }
	bool Exists(const std::filesystem::path& name) const;
private:
	std::unordered_map<std::filesystem::path, Ref<Texture2D>> m_Textures;
};

